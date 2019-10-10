#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H
#include <Seeed_QTouch.h>

class TouchManager
{
public :

  #define NUM_TOUCH 7
  boolean lastTouch[NUM_TOUCH];
  boolean touch[NUM_TOUCH];

  int lastState = 0;

  bool multitouchMode = true;
  byte threshold = 240;
  
  TouchManager()
  {
    addCallbackTouchUpdate(&TouchManager::defaultCallback);

  }

  void init()
  {
    initValues();
//    QTouch.setAKS(!multitouchMode); //Disable Adjacent Key Suppression (allow for multitouch);
    setThreshold(threshold);
  }

   void update()
  {      
    //TODO, uset getState to get all touches
    for(int i=0;i<NUM_TOUCH;i++)
    {
      
      touch[i] = QTouch.isTouch(i);
      if(multitouchMode) touch[i] = !touch[i];
      
      if(touch[i] != lastTouch[i])
      {
        onTouchUpdate(i,touch[i]);
        lastTouch[i] = touch[i];
      }
    }
    
    
  }     
  
  void initValues()
  {
    for(int i=0;i<NUM_TOUCH;i++)
    {
      lastTouch[i] = false;
      touch[i] = false;
    }
  }

  void setThreshold(byte value)
  {
    threshold = value;
    for(int i=0;i<NUM_TOUCH;i++) QTouch.setNTHRForKey(threshold,i); //Disable Adjacent Key Suppression (allow for multitouch);
    DBG("Touch threshold :"+String(value));
  }

  #if USE_OSC
  boolean handleMessage(OSCMessage &msg)
  {
    
    if(msg.match("/touch/reset"))
    {
      QTouch.reset(); 
//      QTouch.setAKS(!multitouchMode);
      initValues();
      setThreshold(threshold);
      
      return true;
    }else if(msg.match("/touch/resetValues"))
    {
      initValues();      
      return true;
    }else if(msg.match("/touch/calibrate"))
    {
      QTouch.calibrate();
//      QTouch.setAKS(!multitouchMode);
      setThreshold(threshold);
      
      return true;
    }else if(msg.match("/touch/multitouch"))
    {
      multitouchMode = msg.getInt(0) != 0;
//      QTouch.setAKS(!multitouchMode); //Disable Adjacent Key Suppression (allow for multitouch);
      setThreshold(threshold);
      return true;
    }else if(msg.match("/touch/threshold"))
    {
      setThreshold(msg.getFloat(0)*255);
      return true;
    }
  }
  #endif
  

  typedef void(*onEvent)(int,bool);
  void (*onTouchUpdate) (int,bool);
  
  void addCallbackTouchUpdate (onEvent func) { 
    onTouchUpdate = func;
  }

  static void defaultCallback(int touchID, bool touched)
  {
    //nothing
  }
  
  
};

#endif
