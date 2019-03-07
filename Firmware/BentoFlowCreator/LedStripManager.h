#ifndef LEDSTRIP_MANAGER_H
#define LEDSTRIP_MANAGER_H

#define LED_ENABLE_PIN 26

#define FASTLED_ALLOW_INTERRUPTS 0 //if not working here, modify fastled_config.h to uncomment this same line
#include "FastLED.h"
#include "FastLEDDefs.h"

#define DATA_PIN 5
#define CLK_PIN 18
#define LED_TYPE    SK9822
#define COLOR_ORDER BGR

#include "PatternManager.h"

#if USE_FILES
#include "BakePlayer.h"
#endif

#define NUM_MODES 2

class LedStripManager
{
public:

  enum Mode { Streaming, Pattern, Baked };
  Mode currentMode;

  PatternManager pm;

  #if USE_FILES
  BakePlayer bakePlayer;
  #endif
  
  int propIDBufferIndex;
  int propBufferIndex;
  
  int ledBufferIndex;
  uint8_t colorBuffer[3];
  int colorBufferIndex;

  //tempo
  int receiveRate;
  long lastReceiveTime;
  
  //stats
  long lastStatTime;
  int numPacketsSinceLastTime;
  long timeSinceLastPacket;
  long numLoops;
  
  #if USE_WIFI
  WiFiUDP &udp;
  LedStripManager(WiFiUDP &udp):udp(udp)
  #else
  LedStripManager()
  #endif
  {
    currentMode = Mode::Streaming;

    receiveRate = 60;
    lastReceiveTime = 0;
    
    lastStatTime = 0;
    numPacketsSinceLastTime = 0;
  
    propIDBufferIndex = 0;
    propBufferIndex = 0;
    ledBufferIndex = 0;
    colorBufferIndex = 0;
    for(int i=0;i<3;i++) colorBuffer[i] = 0;
  }
  
  void init(){
    DBG("LedStripManager Init.");

    pinMode(LED_ENABLE_PIN, OUTPUT); //LED's enable
    digitalWrite(LED_ENABLE_PIN, HIGH);

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
        processWiFi();
        #endif
      break;

      case Mode::Pattern:
        pm.update();
        break;

      case Mode::Baked:
      #if USE_FILES
        bakePlayer.update();
      #endif
        break;
    }
  }
  
  
  #if USE_WIFI
  void processWiFi()
  {
    long curTime = millis();
    if(curTime > lastReceiveTime + (1000/receiveRate))
    {
      receiveUDP();
      lastReceiveTime = curTime;
    }
  }

  void receiveUDP()
  {
    if (udp.parsePacket())
    {
      while (udp.available() > 0)
      {
        byte b = udp.read();
  
        if (b == 255) processBuffer();
        else
        {
          
          if(propIDBufferIndex == DeviceSettings::propID)
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

          propBufferIndex++;
          if(propBufferIndex == NUM_LEDS*3)
          {
            propIDBufferIndex++;
            propBufferIndex =0;
          }
        }
       
      }
      //udp.flush();
    }

     /*
    numLoops++;
    if(millis() > lastStatTime + 1000)
    {
     lastStatTime = millis();
     Serial.println(String(numPacketsSinceLastTime)+ " updates/s,\t\t"+String(numLoops)+" loops/s");
     numLoops = 0;
     numPacketsSinceLastTime = 0;
    }
    */
    
  }
  
  void processBuffer()
  {
    /*
    Serial.println("");
    Serial.print("process Buffer : ");
    Serial.print(ledBufferIndex);
    Serial.println(" received.");
    */
    //FastLED.clear();
    FastLED.show();
    
    propIDBufferIndex = 0;
    propBufferIndex = 0;
    ledBufferIndex = 0;
    /*
    long t = millis();
    Serial.println("Process "+String(t-timeSinceLastPacket));
    timeSinceLastPacket = t;
  numPacketsSinceLastTime++;6
   */
   
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
    
    DBG("LedStripManager set Mode : "+String(currentMode));

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

    char addr[32];
    int len = 0;
    msg.getAddress(addr,len);
    Serial.println("Got message for strip "+String(addr));
    if(msg.match("/fps", offset))
    {
      if(msg.size() > 0 && msg.isInt(0))
      {
        receiveRate = min(max(msg.getInt(0),1),1000);
        Serial.println("Receive rate : "+String(receiveRate));
      }
    }else if(msg.match("/color",offset)) setFullColor(CRGB(msg.getFloat(0)*255,msg.getFloat(1)*255,msg.getFloat(2)*255));
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
      }else 
      {
      
      #if USE_FILES
      if(bakePlayer.handleMessage(msg, offset))
      {
        setMode(Baked);
        return true;
      }
      #endif
      
      }
      
      return false;
    }

    return true;
  }
  #endif
};

#endif
