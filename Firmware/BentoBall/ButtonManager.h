#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#define BUTTON_PIN 23
#define LONGPRESS_TIME 500 //more than 500ms is long press
#define VERYLONGPRESS_TIME 1500
#define SHORTPRESS_TIME 500 //less than 500ms is short press
#define MULTIPRESS_TIME 300 //each new press shorter than 500ms after the previous one will increase the multiclick

#define BT_PRESSED 0
#define BT_RELEASED 1
#define BT_SHORTPRESS 2
#define BT_LONGPRESS 3
#define BT_VERYLONGPRESS 4

#define BT_PRESS_DEBOUNCE 5

class ButtonManager
{
public:
  bool pressed;
  bool longPress;
  bool veryLongPress;
  long timeAtPress;
  int multipressCount;
  int debounceCount;

  
  void init()
  {
    pressed = false;
    longPress = false;
    veryLongPress = false;
    timeAtPress = 0;
    debounceCount = 0;
    addButtonCallback(&ButtonManager::onButtonEventDefaultCallback);
    addMultipressCallback(&ButtonManager::onMultipressDefaultCallback);
    
    pinMode(BUTTON_PIN, INPUT);
  }

  
  void update()
  {
    bool v = buttonIsPressed();
    
    if(v) debounceCount = min(debounceCount+1,BT_PRESS_DEBOUNCE);
    else debounceCount = max(debounceCount-1, 0);

    bool newPressed = pressed;
    if(pressed && debounceCount ==  0) newPressed = false;
    if(!pressed && debounceCount == BT_PRESS_DEBOUNCE)  newPressed = true; 
       
    if(newPressed != pressed) //button state changed
    {
      pressed = newPressed;
      longPress = false;
      veryLongPress = false;
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
        if(!longPress && millis() > timeAtPress + LONGPRESS_TIME)
        {
          longPress = true;
          onButtonEvent(BT_LONGPRESS);
        }

        if(!veryLongPress && millis() > timeAtPress + VERYLONGPRESS_TIME)
        {
          veryLongPress = true;
          onButtonEvent(BT_VERYLONGPRESS);
        }
      }

      if(millis() > timeAtPress + MULTIPRESS_TIME) multipressCount = 0;
    }
  }
  
  bool buttonIsPressed()
  {
    return digitalRead(BUTTON_PIN);
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

#endif
