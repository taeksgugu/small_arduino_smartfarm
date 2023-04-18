#define PUMP 16

void setup() {
  pinMode(PUMP, OUTPUT);
}

void loop() {
  digitalWrite(PUMP, HIGH);  /* digital에서는 High가 1, Low가 0이므로 전원을 뜻함*ㄴ/
  delay(2000);
  digitalWrite(PUMP, LOW);
  delay(2000);
}
