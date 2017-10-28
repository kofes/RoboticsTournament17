#include <Wire.h>
#include <VirtualWire.h>
#include "SparkFun_APDS9960.h"

#define PWMA 3
#define AIN2 4
#define AIN1 5
#define STBY 6 
#define BIN1 7
#define BIN2 8
#define PWMB 9
#define CONTROL_LINE_WHITE  0
#define CONTROL_LINE_LEFT   1
#define CONTROL_LINE_RIGHT  2
#define ANALOG_SENSOR_PIN_LEFT  A0 
#define ANALOG_SENSOR_PIN_RIGHT  A1 
#define LEDPIN    13
#define A_MOTOR_SPEED         255
#define B_MOTOR_SPEED         235
#define DELTA_MOTOR_SPEED     25
#define MINIMUM_MOTOR_SPEED   50
#define MAKSIMUM_MOTOR_SPEED   255
#define MINIMUM_MOTOR_SPEED_IK   0
#define MAKSIMUM_MOTOR_SPEED_IK   150 
#define IK_SENSOR    500

float MOTOR_LEFT_COEF  = (float)255/255;
float MOTOR_RIGHT_COEF = (float)225/255;

int AmotorSpeed; //  скорость мотора
int BmotorSpeed; //  скорость мотора
bool boost;
unsigned long time_m;
int typeControlLine;
int programmPart;


#define GREEN 1
#define YELLOW 2
#define BLUE 4
#define RED 8

SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

typedef struct APDS {
  char Pin;
  uint16_t ambient_light = 0;
  uint16_t red_light = 0;
  uint16_t green_light = 0;
  uint16_t blue_light = 0;
} APDS;

typedef struct RF_5V {//приёмник
  char Resiver;
  byte message[VW_MAX_MESSAGE_LEN];
  byte messageLength = VW_MAX_MESSAGE_LEN; 
} RF_5V;

//<------------------------------------------------
typedef struct MH_SS {
  char Analog, Digital;
} MH_SS;

static struct MH_SS SS_left, SS_right;

unsigned int MH_SS_Analog_Loop(struct MH_SS* in) {
  return analogRead(in->Analog);
}

unsigned int MH_SS_Digital_Loop(struct MH_SS* in) {
  return digitalRead(in->Digital);
}


void BMototr(bool motorDirection, int motorSpeed)
{
     digitalWrite(BIN1, motorDirection);
     digitalWrite(BIN2, !motorDirection);    
     analogWrite(PWMB, motorSpeed); 
}

void AMototr(bool motorDirection, int motorSpeed)
{
     digitalWrite(AIN1, motorDirection);
     digitalWrite(AIN2, !motorDirection);
     analogWrite(PWMA, motorSpeed); 
}

void MotorPower(bool motorPower)
{
     digitalWrite(STBY, motorPower);     // выключаем
}

void ControlLine()
{
    int  switch_state_left, switch_state_right;
    bool bool_state_left, bool_state_right;;
    switch_state_left = MH_SS_Analog_Loop(&SS_left);
    switch_state_right = MH_SS_Analog_Loop(&SS_right);
    bool_state_left = (switch_state_left < IK_SENSOR)?false:true;
    bool_state_right = (switch_state_right < IK_SENSOR)?false:true;
    //Serial.print(bool_state_left);
    //Serial.print(" ");
    //Serial.println(bool_state_right);
    //Serial.print(switch_state_left);
    //Serial.print(" ");
    //Serial.println(switch_state_right);
    if (bool_state_left == bool_state_right)
    {
        if (typeControlLine != CONTROL_LINE_WHITE)
        {
            time_m = millis();
            boost = false;
            typeControlLine = CONTROL_LINE_WHITE;
        }
        else if (millis() - time_m > 3000)
        {
            boost = !boost;
            time_m = millis();
        }
        if (!boost)
        {
            AmotorSpeed = MAKSIMUM_MOTOR_SPEED_IK; //  скорость мотора
            BmotorSpeed = MAKSIMUM_MOTOR_SPEED_IK; //  скорость мотора
        }
        else
        {
            AmotorSpeed = MAKSIMUM_MOTOR_SPEED; //  скорость мотора
            BmotorSpeed = MAKSIMUM_MOTOR_SPEED; //  скорость мотора
            if (millis() - time_m > 1500) boost = false;
        }
    }
    else if ((bool_state_left)&&(!bool_state_right))
    {
        if (typeControlLine != CONTROL_LINE_LEFT)
        {
            time_m = millis();
            boost = false;
            typeControlLine = CONTROL_LINE_LEFT;
        }
        else if (millis() - time_m > 3000)
        {
            boost = !boost;
            time_m = millis();
        }
        AmotorSpeed = AmotorSpeed - DELTA_MOTOR_SPEED;
        if (AmotorSpeed < MINIMUM_MOTOR_SPEED_IK) AmotorSpeed = MINIMUM_MOTOR_SPEED_IK;
        BmotorSpeed = MAKSIMUM_MOTOR_SPEED_IK + 50; //  скорость мотора
        if (boost)
        {
            BmotorSpeed = MAKSIMUM_MOTOR_SPEED; //  скорость мотора
            if (millis() - time_m > 1500) boost = false;
        }
    }
    else if ((!bool_state_left)&&(bool_state_right))
    {
        if (typeControlLine != CONTROL_LINE_RIGHT)
        {
            time_m = millis();
            boost = false;
            typeControlLine = CONTROL_LINE_RIGHT;
        }
        else if (millis() - time_m > 3000)
        {
            boost = !boost;
            time_m = millis();
        }
        BmotorSpeed = BmotorSpeed - DELTA_MOTOR_SPEED;
        if (BmotorSpeed < MINIMUM_MOTOR_SPEED_IK) BmotorSpeed = MINIMUM_MOTOR_SPEED_IK;
        AmotorSpeed = MAKSIMUM_MOTOR_SPEED_IK + 50; //  скорость мотора
        if (boost)
        {
            AmotorSpeed = MAKSIMUM_MOTOR_SPEED; //  скорость мотора
            if (millis() - time_m > 1500) boost = false;
        }
    }
    AMototr(HIGH, AmotorSpeed);
    BMototr(HIGH, BmotorSpeed);
}

/////////////////COLORS
static struct APDS s_apds;
static char state;

byte colorMask = 0;

bool isBlue(struct APDS* apds) {
  return apds->blue_light >= apds->red_light && apds->blue_light >= apds->green_light;
}
bool isRed(struct APDS* apds) {
  return apds->red_light >= apds->blue_light && apds->red_light >= apds->green_light;
}
bool isGreen(struct APDS* apds) {
  return apds->green_light >= apds->red_light && apds->green_light >= apds->blue_light;
}

const size_t size_amb = 4;
static double amb[size_amb];
static byte last = 0;
static bool isComplete = false;

void colorControl(struct APDS* apds) {
  if (!isComplete && last < size_amb) {
    amb[last++] = apds->ambient_light;
  } else {
    isComplete = true;
    last = (last+1) % size_amb;
    amb[last] = apds->ambient_light;
    byte ppv = 3, pv = 2, v = 1;
    double prev = (amb[(last+ppv)%size_amb] +
                   amb[(last+pv)%size_amb] +
                   amb[(last+v)%size_amb])/3;
    //From black to green
    if (state == 'B' && isGreen(apds) && prev*1.2 <= amb[last]) {
      state = 'G';
      isComplete = false;
      last = 0;
    //From green to white
    } else if (state == 'G' && prev*1.3 <= amb[last]) {
      state = 'W';
      isComplete = false;
      last = 0;
    //From white to yellow
    } else if (state == 'W' && isGreen(apds) && prev >= 1.2 * amb[last]) {
      state = 'Y';
      isComplete = false;
      last = 0;
    //From yellow to gray
    } else if (state == 'Y' && prev >= amb[last] * 1.2) {
      state = 'g';
      isComplete = false;
      last = 0;
    //From gray to blue
    } else if (state == 'g' && isBlue(apds) && prev >= amb[last]*1.2) {
      state = 'b';
      isComplete = false;
      last = 0;
    //From blue to white
    } else if (state == 'b' && prev*1.3 <= amb[last]) {
      state = 'W';
    //From white to red
    } else if (state == 'W' && isRed(apds) && prev >= amb[last]*1.4) {
      state = 'R';
    }
  }
  Serial.println(state);
  switch (state) {
    case 'B' : Serial.println("Black"); break;
    case 'G' : Serial.println("Green"); break;
    case 'W' : Serial.println("White"); break;
    case 'Y' : Serial.println("Yellow"); break;
    case 'g' : Serial.println("Gray"); break;
    case 'b' : Serial.println("Blue"); break;
    case 'R' : Serial.println("Red"); break;
    default: Serial.println("Unknown color"); break;
  }

  if(state == 'G') {
    programmPart = 1;
    //colorState = 'G';
    //colorMask |= GREEN;
    //MAX SPEED
    if (isComplete) {
      AMototr(HIGH, MAKSIMUM_MOTOR_SPEED*MOTOR_LEFT_COEF);
      BMototr(HIGH, MAKSIMUM_MOTOR_SPEED*MOTOR_RIGHT_COEF);
      delay(2000);
      AMototr(HIGH, MINIMUM_MOTOR_SPEED*MOTOR_LEFT_COEF);
      BMototr(HIGH, MINIMUM_MOTOR_SPEED*MOTOR_RIGHT_COEF);
    } else {
      //STOP
      AMototr(HIGH, 0);
      BMototr(HIGH, 0);
    }
  } else if(state == 'Y') {
    //colorState = 'Y';
    //colorMask |= YELLOW;
    //MOVE SLOW
    if (isComplete) {
      AMototr(HIGH, MINIMUM_MOTOR_SPEED*MOTOR_LEFT_COEF);
      BMototr(HIGH, MINIMUM_MOTOR_SPEED*MOTOR_RIGHT_COEF);
    } else {
      //STOP
      AMototr(HIGH, 0);
      BMototr(HIGH, 0);
    }
  } else if(state == 'R') {
    //colorState = 'R';
    //colorMask |= RED;
    //STOP
    AMototr(HIGH, 0);
    BMototr(HIGH, 0);
  } else if(state == 'b') {
    //colorState = 'B';
    //colorMask |= BLUE2;
  }

  //if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) < 500 && MH_SSAnalogLoop(&SS_Right) > 500) {
    //TRUT to right
  //}
  //if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) > 500 && MH_SSAnalogLoop(&SS_Right) < 500) {
    //TRUT to left
  //}
  //if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) < 500 && MH_SSAnalogLoop(&SS_Right) < 500) {
    //MOVE SLOW
    //AMotor(HIGH, );
    //BMotor(HIGH, );
  //}
}
void APDS_Setup(struct APDS* in) {
    Serial.println("Alert!");
  attachInterrupt(0, interruptRoutine, FALLING);
    Serial.println("Alert!");
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  }
    Serial.println("Alert!");
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  }
    Serial.println("Alert!");
  delay(500);
}

bool APDS_Loop(struct APDS* in) {
  return !(
    !apds.readAmbientLight(in->ambient_light) ||
    !apds.readRedLight(in->red_light) ||
    !apds.readGreenLight(in->green_light) ||
    !apds.readBlueLight(in->blue_light)
  );
}

void interruptRoutine() {
  isr_flag = 1;
}


static struct RF_5V strGet;

void RF_5V_Setup(struct RF_5V* in) {
  vw_set_rx_pin(in->Resiver);
  vw_setup(2000);
  vw_rx_start();
}

void RF_5V_GetMsg_Loop(struct RF_5V* in) {
  if (vw_get_message(in->message, &(in->messageLength))) { 
    if(in->messageLength == 1 && in->message[0] == 0xFE) {
      AMototr(HIGH, 0);
      BMototr(HIGH, 0);
    }
    
    //Serial.println("!!!!\n"); 
    //digitalWrite(led_pin, HIGH);
    //for (int i = 0; i < in->messageLength; i++) { 
    //  Serial.write(in->message[i]);
    //} 
    //digitalWrite(led_pin, LOW);
  } 
}
////////END COLORS

void setup(){
    delay(3000);
    Serial.begin(9600);
    Serial.print(MOTOR_RIGHT_COEF);
    pinMode(PWMA, OUTPUT);  
    pinMode(PWMB, OUTPUT);  
    pinMode(AIN1, OUTPUT);  
    pinMode(AIN2, OUTPUT);  
    pinMode(BIN1, OUTPUT);  
    pinMode(BIN2, OUTPUT);  
    pinMode(STBY, OUTPUT);  
    AmotorSpeed = MAKSIMUM_MOTOR_SPEED_IK; //  скорость мотора
    BmotorSpeed = MAKSIMUM_MOTOR_SPEED_IK; //  скорость мотора
    MotorPower(HIGH); 
    AMototr(HIGH, AmotorSpeed);
    BMototr(HIGH, BmotorSpeed);
    SS_left.Analog = ANALOG_SENSOR_PIN_LEFT;
    SS_right.Analog = ANALOG_SENSOR_PIN_RIGHT;
    boost = false;
    typeControlLine = CONTROL_LINE_WHITE;
    time_m = millis();
    programmPart = 0;
    state = 'B';//First color - black
    s_apds.Pin = 2;
    APDS_Setup(&s_apds);

    //for radio
    //strGet.Resiver = 11; 
    //RF_5V_Setup(&strGet);
}

void treadControlLine(int n){
    for (int i = 0; i < n; i++)
    {
        delay(1);
    }
}

void loop(){
    if (programmPart == 0)
    {
        ControlLine();
    }
    bool isOk = APDS_Loop(&s_apds);
    if (!isOk)
        Serial.println("Error reading light values");
    else
        colorControl(&s_apds);
    //for radio
    //RF_5V_GetMsg_Loop(&strGet);
  /*
     //BMototr(LOW, motorSpeedGlobal);
     AMototr(LOW, motorSpeedGlobal);
     delay(1000);
     //BMototr(HIGH, motorSpeedGlobal);
     AMototr(HIGH, motorSpeedGlobal);
     delay(1000);
     MotorPower(LOW);
     delay(1000);
     MotorPower(HIGH);
     */
} 

