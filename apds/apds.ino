#include <Wire.h>
#include "SparkFun_APDS9960.h"

SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

typedef struct APDS {
  char Pin;
  uint16_t ambient_light = 0;
  uint16_t red_light = 0;
  uint16_t green_light = 0;
  uint16_t blue_light = 0;
} APDS;
static struct APDS str;


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

void APDS_Loop(struct APDS* in) {
  if ( !apds.readAmbientLight(in->ambient_light) ||
    !apds.readRedLight(in->red_light) ||
    !apds.readGreenLight(in->green_light) ||
    !apds.readBlueLight(in->blue_light) ) {

    Serial.println("Error reading light values");

  } else {
    Serial.print("Ambient: ");
    Serial.println(in->ambient_light);
    Serial.print(" Red: ");
    Serial.println(in->red_light);
    Serial.print(" Green: ");
    Serial.println(in->green_light);
    Serial.print(" Blue: ");
    Serial.println(in->blue_light);
    Serial.println();
  }
}

void setup() {
  str.Pin = 2;
  Serial.begin(9600);
  
  APDS_Setup(&str);
}

void loop() {
  APDS_Loop(&str);
  delay(1000);
}

void interruptRoutine() {
  isr_flag = 1;
}


