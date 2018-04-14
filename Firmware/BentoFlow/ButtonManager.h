#define BUTTON_PIN 13
#define LONGPRESS_TIME 700 //more than 500ms is long press
#define SHORTPRESS_TIME 500 //less than 500ms is short press
#define MULTIPRESS_TIME 250 //each new press shorter than 500ms after the previous one will increase the multiclick

#define BT_PRESSED 0
#define BT_RELEASED 1
#define BT_SHORTPRESS 2
#define BT_LONGPRESS 3

class ButtonManager
{
public:
  bool pressed;
  bool longPress;
  long timeAtPress;
  int multipressCount;
  
  void init()
  {
    pressed = false;
    longPress = false;
    timeAtPress = 0;
    addButtonCallback(&ButtonManager::onButtonEventDefaultCallback);
    addMultipressCallback(&ButtonManager::onMultipressDefaultCallback);
    
    pinMode(BUTTON_PIN, INPUT);
  }

  bool buttonIsPressed()
  {
    return digitalRead(BUTTON_PIN);
  }

  void update()
  {
    bool v = digitalRead(BUTTON_PIN);
    if(v != pressed) //button state changed
    {
      pressed = v;
      longPress = false;
      if(pressed) 
      {
        timeAtPress = millis();
        onButtonEvent(BT_PRESSED);

        multipressCount ++;
        if(multipressCount > 1) onMultipress(multipressCount);
      }else
      {
        if(millis() < timeAtPress + SHORTPRESS_TIME) onButtonEvent(BT_SHORTPRESS);
        onButtonEvent(BT_RELEASED);
      }
    }else
    {
      if(pressed)
      {
        if(millis() > timeAtPress + LONGPRESS_TIME)
        {
          onButtonEvent(BT_LONGPRESS);
        }
      }

      if(millis() > timeAtPress + MULTIPRESS_TIME) multipressCount = 0;
    }
  }

  typedef void(*ButtonEvent)(int type);
  void (*onButtonEvent) (int type);
  void addButtonCallback (ButtonEvent func) { 
    onButtonEvent = func;
  }
  static void onButtonEventDefaultCallback(int type) {}

  
  typedef void(*MultipressEvent)(int count);
  void (*onMultipress) (int count);
  void addMultipressCallback (MultipressEvent func) { 
    onMultipress = func;
  }
  static void onMultipressDefaultCallback(int count) {}
};

