#include <VirtualWire.h>

const int led_pin = 13;

//<------------------------------
typedef struct FS1_000A {
  char Transmit;
} FS1_000A;

static struct FS1_000A strSend;

void FS1_000A_Setup(struct FS1_000A* in) {
  vw_set_tx_pin(in->Transmit); 
  vw_setup(2000);
  return;
}

void FS1_000A_SendMsg_Loop(const char *msg) {
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();
}

//----------------------------->
void setup() {
  Serial.begin(9600);
  
  strSend.Transmit = 8;
  strGet.Resiver = 10; 
  FS1_000A_Setup(&strSend);
  
  pinMode(led_pin, OUTPUT); 
}

void loop() {
  const char *msg = "LOL KEK!!!";
  
  digitalWrite(led_pin, HIGH);
  FS1_000A_SendMsg_Loop(msg);
  digitalWrite(led_pin, LOW);
  
  
  delay(1000);
}


