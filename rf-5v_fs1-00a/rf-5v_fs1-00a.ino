#include <VirtualWire.h>

const int led_pin = 13;

//<------------------------------
typedef struct FS1_000A {
  char Transmit;
} FS1_000A;

static struct FS1_000A strSend;

typedef struct RF_5V {//приёмник
  char Resiver;
  byte message[VW_MAX_MESSAGE_LEN];
  byte messageLength = VW_MAX_MESSAGE_LEN; 
} RF_5V;

static struct RF_5V strGet;

void FS1_000A_Setup(struct FS1_000A* in) {
  vw_set_tx_pin(in->Transmit); 
  vw_setup(2000);
  return;
}

void FS1_000A_SendMsg_Loop(const char *msg) {
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();
}



void RF_5V_Setup(struct RF_5V* in) {
  vw_set_rx_pin(in->Resiver);
  vw_setup(2000);
  vw_rx_start(); 
  return;
}

void RF_5V_GetMsg_Loop(struct RF_5V* in) {
  if (vw_get_message(in->message, &in->messageLength)) { 
    digitalWrite(led_pin, HIGH);
    for (int i = 0; i < in->messageLength; i++) { 
      Serial.write(in->message[i]);
    } 
    Serial.println(); 
    digitalWrite(led_pin, LOW);
  } 
}
//----------------------------->
void setup() {
  Serial.begin(9600);
  
  strSend.Transmit = 8;
  strGet.Resiver = 10; 
  FS1_000A_Setup(&strSend);
  RF_5V_Setup(&strGet);
  
  pinMode(led_pin, OUTPUT); 
}

void loop() {
  const char *msg = "LOL KEK!!!";
  
  digitalWrite(led_pin, HIGH);
  FS1_000A_SendMsg_Loop(msg);
  digitalWrite(led_pin, LOW);
  
  RF_5V_GetMsg_Loop(&strGet);
  
  delay(1000);
}


