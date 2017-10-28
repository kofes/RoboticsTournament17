
#define ANALOG_SENSOR_PIN  0 
#define DIGITAL_SENSOR_PIN 8
#define LEDPIN    13

int  switch_state;
int  LightAnalogValue; 
//<------------------------------------------------
typedef struct MH_SS {
  char Analog, Digital;
} MH_SS;

static struct MH_SS str;

unsigned int MH_SS_Analog_Loop(struct MH_SS* in) {
  return analogRead(in->Analog);
}

unsigned int MH_SS_Digital_Loop(struct MH_SS* in) {
  return digitalRead(in->Digital);
}
//--------------------------------------------------->
void setup() {
  str.Analog = 0;
  str.Digital = 8;
  
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  switch_state = MH_SS_Digital_Loop(&str);
  if (switch_state == LOW) {
    digitalWrite(LEDPIN, HIGH);
  } else {
    digitalWrite(LEDPIN, LOW);
  }

  LightAnalogValue = switch_state = MH_SS_Analog_Loop(&str);
  Serial.println(LightAnalogValue,DEC);
  delay(500);
}
