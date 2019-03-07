long cTime = 0;
long aTime = 0;
long pTime = 0;
long timeAtStart;

long timeSinceLastSend;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  cTime = millis();
  timeSinceLastSend = millis();
}

void loop() 
{
  long mil = millis();
  
  cTime += mil - pTime;
  pTime = mil;
 
  aTime = mil-timeAtStart;

  if(mil > timeSinceLastSend+50)
  {
    Serial.println("val "+String((float)(cTime/1000.0f)));
    Serial.println("val2 "+String((float)(aTime/1000.0f)));
    timeSinceLastSend = mil;
  }
  
  if(Serial.available() > 0)
  {
    char c = Serial.read();
    if(c == 'r') 
    {
      cTime = 0;
      timeAtStart = millis();
    }
  }
}
