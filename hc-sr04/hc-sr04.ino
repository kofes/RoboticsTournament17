#define ledPin 13

typedef struct HC_SR04 {
  char Trig, Echo;
} HC_SR04;

void HC_SR04_Setup(struct HC_SR04* in) {
  pinMode(in->Trig, OUTPUT);
  pinMode(in->Echo, INPUT);
  pinMode(ledPin, OUTPUT);
}

unsigned int HC_SR04_Loop(struct HC_SR04* in) {
  digitalWrite(in->Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(in->Trig, LOW);
  unsigned int distTime = pulseIn(in->Echo, HIGH);
  return distTime / 58;
}

static struct HC_SR04 str;

void setup() {
  str.Trig = 9;
  str.Echo = 8;
  // put your setup code here, to run once:
  HC_SR04_Setup(&str);
  Serial.begin(9600);
} 

unsigned int distTime = 0;
unsigned int distance = 0;

void loop() {

  distance = HC_SR04_Loop(&str);
  
  Serial.println(distance);

  if(distance < 20) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  delay(100);
  
}

