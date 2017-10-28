#define PWMA 3
#define AIN2 4
#define AIN1 5
#define STBY 6 
#define BIN1 7
#define BIN2 8
#define PWMB 9


int motorSpeedGlobal = 100; //  скорость мотора

void setup(){

    pinMode(PWMA, OUTPUT);  
    pinMode(PWMB, OUTPUT);  
    pinMode(AIN1, OUTPUT);  
    pinMode(AIN2, OUTPUT);  
    pinMode(BIN1, OUTPUT);  
    pinMode(BIN2, OUTPUT);  
    pinMode(STBY, OUTPUT);  
    MotorPower(HIGH); 
 
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

void loop()
{
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

