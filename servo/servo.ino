#include <Servo.h>

Servo servo;

#define SERVO_PIN 10

void setup() {
  servo.attach(SERVO_PIN);
}

void loop() {
  servo.write(0);
  delay(2000);
  servo.write(180);
  delay(2000);
}
