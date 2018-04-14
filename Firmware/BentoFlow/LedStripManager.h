#ifndef LEDSTRIP_MANAGER_H
#define LEDSTRIP_MANAGER_H

#define LED_ENABLE_PIN 9

#define FASTLED_ALLOW_INTERRUPTS 0 //if not working here, modify fastled_config.h to uncomment this same line
#include "FastLED.h"
#include "FastLEDDefs.h"

#define DATA_PIN 5
#define CLK_PIN 4
#define LED_TYPE    APA102
#define COLOR_ORDER BGR

#include "PatternManager.h"

#define NUM_MODES 2

class LedStripManager
{
public:

  enum Mode { Streaming, Pattern };
  Mode currentMode;

  PatternManager pm;

  int ledBufferIndex;
  uint8_t colorBuffer[3];
  int colorBufferIndex;
  
  #if USE_WIFI
  WiFiUDP &udp;
  LedStripManager(WiFiUDP &udp):udp(udp)
  #else
  LedStripManager()
  #endif
  {
    currentMode = Mode::Streaming;
    
    ledBufferIndex = 0;
    colorBufferIndex = 0;
    for(int i=0;i<3;i++) colorBuffer[i] = 0;
  }
  
  void init(){
    #if SERIAL_DEBUG
    Serial.println("LedStripManager Init.");
    #endif

    pinMode(LED_ENABLE_PIN, OUTPUT); //LED's enable
    digitalWrite(LED_ENABLE_PIN, LOW);

    FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(60);  

    initSnapshot();
  }

  void update()
  {
    switch(currentMode)
    {
      case Mode::Streaming:
        #if USE_WIFI
        processWifi();
        #endif
      break;

      case Mode::Pattern:
        pm.update();
        break;
    }
      
  }
  
  
  #if USE_WIFI
  void processWifi()
  {
    if (udp.parsePacket())
    {
      while (udp.available() > 0)
      {
        byte b = udp.read();
  
        if (b == 255) processBuffer();
        else if (ledBufferIndex < NUM_LEDS)
        {
          colorBuffer[colorBufferIndex] = (uint8_t)b;
          colorBufferIndex++;
          if (colorBufferIndex == 3)
          {
            leds[ledBufferIndex] = CRGB(colorBuffer[0], colorBuffer[1], colorBuffer[2]);

            //Serial.print(leds[ledBufferIndex],DEC);
            //Serial.print(" ");
            colorBufferIndex = 0;
            ledBufferIndex ++;
          }
        }
      }
      udp.flush();
    }
  
    delay(2);
    //udp.send("o 1.2543487 1.214657 0.312354589");
  }
  
  void processBuffer()
  {
    //Serial.println("");
    //Serial.print("process Buffer : ");
    //Serial.print(ledBufferIndex);
    //Serial.println(" received.");
    //FastLED.clear();
    FastLED.show();
    ledBufferIndex = 0;
  }
  #endif

  void nextMode()
  {
    setMode((Mode)((currentMode+1)% NUM_MODES));
  }

  void setMode(Mode newMode)
  {
    if(currentMode == newMode) return;
    
    currentMode = newMode;
    
    #if SERIAL_DEBUG
    Serial.print("LedStripManager set Mode : ");
    Serial.println(currentMode);
    #endif

    FastLED.clear();
    FastLED.show();
    
    switch(currentMode)
    {
      case Streaming:
      break;

      case Pattern:
      pm.setPattern(PatternManager::Snapshot);
      break;
    }
  }
  
  #if USE_OSC
  boolean handleMessage(OSCMessage &msg)
  { 
    int offset = msg.match("/strip");
    
    
    if(offset == 0) return false;
    
    if(msg.match("/color",offset)) setFullColor(CRGB(msg.getFloat(0)*255,msg.getFloat(1)*255,msg.getFloat(2)*255));
    else if(msg.match("/brightness",offset)) 
    {
      FastLED.setBrightness(msg.getFloat(0)*255);
      FastLED.show();
    }
    else if(msg.match("/mode",offset)) setMode((Mode)msg.getInt(0));
    else if(msg.match("/snap",offset))
    {
      takeSnapshot();
      setMode(Pattern);
      pm.setPattern(PatternManager::Snapshot);
      
    }
    else
    {
      if(pm.handleMessage(msg,offset)) 
      {
        setMode(Pattern);
        return true;
      }
      
      return false;
    }

    return true;
  }
  #endif
};

#endif
