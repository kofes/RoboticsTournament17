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
    Serial.println("<color>");
      Serial.print("\t<ambient>");
        Serial.print(in->ambient_light);
      Serial.println("</ambient>");
      Serial.print("\t<red>");
        Serial.print(in->red_light);
      Serial.println("</red>");
      Serial.print("\t<green>");
        Serial.print(in->green_light);
      Serial.println("</green>");
      Serial.print("\t<blue>");
        Serial.print(in->blue_light);
      Serial.println("</blue>");
    Serial.println("</color>");
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


