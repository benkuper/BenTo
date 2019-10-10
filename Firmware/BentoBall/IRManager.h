#define IR_PIN 14
#define IR_RX 34
#define MAX_IR_POWER 25 //10%
#define IR_CHANNEL 0
#define IR_PWM_RESOLUTION 8 //8 = value range from 0 to 255
#define IR_FREQ 5000

class IRManager
{
public:
  IRManager()
  {
  }
  
  float irFactor;
  int irState;
  
  void init()
  {
     DBG("IR Manager init.");
     
      irFactor = 0;
      irState = 0;
      //pinMode(IR_PIN, OUTPUT);
      ledcSetup(IR_CHANNEL, IR_FREQ, IR_PWM_RESOLUTION);
      ledcAttachPin(IR_PIN,IR_CHANNEL);
      pinMode(IR_RX,INPUT);
      ledcWrite(IR_CHANNEL, 0);
  }
  
  void update()
  {
     int state = analogRead(IR_RX);
     if(state != irState)
     {
        irState = state;
        Serial.println("IR State change : "+String(irState));
     }
  }

  void setIR(float value)
  {
    if(irFactor == value) return;
    irFactor = min(max(value,0.f),1.f);
   // analogWrite(IR_PIN, enabled?IR_POWER:0);
    ledcWrite(IR_CHANNEL, (int)(irFactor*MAX_IR_POWER));
  }

   #if USE_OSC
  boolean handleMessage(OSCMessage &msg)
  { 
    if(msg.match("/ir"))
    {
      if(msg.size() > 0)
      {
        if(msg.isInt(0)) setIR(msg.getInt(0));
        else if(msg.isFloat(0)) setIR(msg.getFloat(0));
      }
      return true;
    }

    return false;
  }
  #endif
};
