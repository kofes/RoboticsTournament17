
static struct APDS strRGB;

#define GREEN 1
#define YELLOW 2
#define BLUE 4
#define RED 8

byte colorMask = 0;

bool isBlue() {
  str.RGB.blue_light >
}
bool isRed() {
  
}
bool isGreen() {
  
}
bool isYellow() {
  
}

char colorState = 0;

bool isWhite

void colorControll() {

  if(isGreen() && !(GREEN&colorMask)) {
    colorState = 'G';
    colorMask |= GREEN;
    //MAX SPEED
    AMotor(HIGH, );
    BMotor(HIGH, );
  } else if(isGreen() && (GREEN&colorMask)) {
    colorState = 'Y';
    colorMask |= YELLOW;
    //MOVE SLOW
    AMotor(HIGH, );
    BMotor(HIGH, );
  } else if(isRed  && (RED&colorMask)) {
    colorState = 'R';
    colorMask |= RED;
    //STOP
    AMotor(HIGH, 0);
    BMotor(HIGH, 0);
  } else if(isBlue() && MH_SSAnalogLoop(&SS_Left) < 500 && MH_SSAnalogLoop(&SS_Right) < 500   && (BLUE&colorMask)) {
    colorState = 'B';
    colorMask |= BLUE2;
  }

  if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) < 500 && MH_SSAnalogLoop(&SS_Right) > 500) {
    //TRUT to right
  }
  if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) > 500 && MH_SSAnalogLoop(&SS_Right) < 500) {
    //TRUT to left
  }
  if(colorState == 'B' && MH_SSAnalogLoop(&SS_Left) < 500 && MH_SSAnalogLoop(&SS_Right) < 500) {
    //MOVE SLOW
    AMotor(HIGH, );
    BMotor(HIGH, );
  }
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
