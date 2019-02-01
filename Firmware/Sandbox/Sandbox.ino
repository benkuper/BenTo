void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(13, INPUT);
  pinMode(23, INPUT);
  pinMode(16, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(String(digitalRead(13)) + " " +String(digitalRead(23)) + " " +String(digitalRead(16)));
  delay(100);
}
