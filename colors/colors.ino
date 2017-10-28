#include <Wire.h>
#include "SparkFun_APDS9960.h"

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

static struct APDS s_apds;
static char state;

static const float AMB_B_TO_G = 1.4f;
static const float AMB_G_TO_W = 1.3f;
static const float AMB_W_TO_Y = 1.2f;
static const float AMB_Y_TO_G = 1.2f;
static const float AMB_G_TO_B = 1.2f;
static const float AMB_B_TO_W = 1.3f;
static const float AMB_W_TO_R = 1.4f;

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
    if (state == 'B' && isGreen(apds) && prev*AMB_B_TO_G <= amb[last]) {
      state = 'G';
      isComplete = false;
      last = 0;
    //From green to white
    } else if (state == 'G' && prev*AMB_G_TO_W <= amb[last]) {
      state = 'W';
      isComplete = false;
      last = 0;
    //From white to yellow
    } else if (state == 'W' && isGreen(apds) && prev >= AMB_W_TO_Y * amb[last]) {
      state = 'Y';
      isComplete = false;
      last = 0;
    //From yellow to gray
    } else if (state == 'Y' && prev >= amb[last] * AMB_Y_TO_G) {
      state = 'g';
      isComplete = false;
      last = 0;
    //From gray to blue
    } else if (state == 'g' && isBlue(apds) && prev >= amb[last]*AMB_G_TO_B) {
      state = 'b';
      isComplete = false;
      last = 0;
    //From blue to white
    } else if (state == 'b' && prev*AMB_B_TO_W <= amb[last]) {
      state = 'W';
    //From white to red
    } else if (state == 'W' && isRed(apds) && prev >= amb[last]*AMB_W_TO_R) {
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
//  if(isGreen() && !(GREEN&colorMask)) {
//    colorState = 'G';
//    colorMask |= GREEN;
//    //MAX SPEED
//    AMotor(HIGH, );
//    BMotor(HIGH, );
//  } else if(isGreen() && (GREEN&colorMask)) {
//    colorState = 'Y';
//    colorMask |= YELLOW;
//    //MOVE SLOW
//    AMotor(HIGH, );
//    BMotor(HIGH, );
//  } else if(isRed()  && (RED&colorMask)) {
//    colorState = 'R';
//    colorMask |= RED;
//    //STOP
//    AMotor(HIGH, 0);
//    BMotor(HIGH, 0);
//  } else if(isBlue() && MH_SSAnalogLoop(&SS_Left) < 500 && MH_SSAnalogLoop(&SS_Right) < 500   && (BLUE&colorMask)) {
//    colorState = 'B';
//    colorMask |= BLUE2;
//  }
//
//  if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) < 500 && MH_SSAnalogLoop(&SS_Right) > 500) {
//    //TRUT to right
//  }
//  if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) > 500 && MH_SSAnalogLoop(&SS_Right) < 500) {
//    //TRUT to left
//  }
//  if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) < 500 && MH_SSAnalogLoop(&SS_Right) < 500) {
//    //MOVE SLOW
//    AMotor(HIGH, );
//    BMotor(HIGH, );
//  }
}

///
void APDS_Setup(struct APDS* in) {
  attachInterrupt(0, interruptRoutine, FALLING);
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  }
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  }
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

void setup() {
  state = 'B';//First color - black
  s_apds.Pin = 2;
  Serial.begin(9600);
  
  APDS_Setup(&s_apds);
}

void loop() {
  bool isOk = APDS_Loop(&s_apds);
  if (!isOk)
    Serial.println("Error reading light values");
  else
    colorControl(&s_apds);
  delay(100);
}

void interruptRoutine() {
  isr_flag = 1;
}
