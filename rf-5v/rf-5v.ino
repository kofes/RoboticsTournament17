#include <VirtualWire.h>

const int led_pin = 13;

//<------------------------------

typedef struct RF_5V {//приёмник
  char Resiver;
  byte message[VW_MAX_MESSAGE_LEN];
  byte messageLength = VW_MAX_MESSAGE_LEN; 
} RF_5V;

static struct RF_5V strGet;

void RF_5V_Setup(struct RF_5V* in) {
  vw_set_rx_pin(in->Resiver);
  vw_setup(2000);
  vw_rx_start();
}

void RF_5V_GetMsg_Loop(struct RF_5V* in) {
  if (vw_get_message(in->message, &(in->messageLength))) { 
    Serial.println("!!!!\n"); 
    digitalWrite(led_pin, HIGH);
    for (int i = 0; i < in->messageLength; i++) { 
      Serial.write(in->message[i]);
    } 
    digitalWrite(led_pin, LOW);
  } 
}
//----------------------------->
void setup() {
  Serial.begin(9600);
  strGet.Resiver = 11; 
  RF_5V_Setup(&strGet);
  pinMode(led_pin, OUTPUT); 
}

void loop() {
  RF_5V_GetMsg_Loop(&strGet);
  delay(1000);
}
