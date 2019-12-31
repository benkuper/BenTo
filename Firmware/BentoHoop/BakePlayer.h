#ifndef BAKEPLAYER_H
#define BAKEPLAYER_H

#include "FileManager.h"
#include "FastLEDDefs.h"

#define FRAME_SIZE (NUM_LEDS*4)

#include <ArduinoJson.h>

class BakePlayer
{
public:

  File curFile;
  File metaDataFile;
  
  int fps = 100;
  byte buffer[FRAME_SIZE];

  int groupID = -1;
  int localID = -1;
  CRGB groupColor = CRGB::Black;
  StaticJsonDocument<200> metaData;

  bool isPlaying = false;
  
  long curTimeMs;
  long prevTimeMs;
  
  //File info
  float totalTime;
  long totalFrames;
  long timeSinceLastSend;

  long timeSinceLastSeek;
  float timeToSeek; //used to limit seeking

  bool idMode;
  bool loopShow = false;
  
  BakePlayer()
  {
    idMode = false;
    curTimeMs = 0;
    prevTimeMs = millis();
    
  }

  void update()
  {
    if(!curFile) return;

    if(idMode)
    {
      showIdFrame();
      return;
    }
    
    if(timeToSeek != -1 && millis() > timeSinceLastSeek + 20)
    {
      seek(timeToSeek);
      timeToSeek = -1;
      timeSinceLastSeek = millis();
    }

    if(!isPlaying) return;
    playFrame();
  }

  void playFrame()
  {
    if(curFile.available() < FRAME_SIZE)
    {
      DBG("End of show");
      if(loopShow) 
      {
        DBG("Loop");
        play(0);
      }
    }

    long mil = millis();
    curTimeMs += mil - prevTimeMs; 
    prevTimeMs = mil;

    long fPos = curFile.position();
    long pos = msToBytePos(curTimeMs);

    if(pos < 0) return;
    if(pos < fPos) return; //waiting for frame
    
    while(fPos > pos)
    {
      curFile.read(buffer,FRAME_SIZE);
      DBG("SKIPPED FRAME");
    }

    if(fPos != pos)
    {
      DBG("SHOULD BE EXACTLY POS "+String(curFile.position())+", expected "+String(pos));
    }

  /*  if(mil > timeSinceLastSend + 100)
    {
      DBG("Playing "+String(msToSeconds(curTimeMs))+"\t Frame : "+String(msToFrame(curTimeMs))
          +"\t, Color "+String(buffer[2])+"\t "+String(buffer[1])+"\t "+String(buffer[0])
        );
        timeSinceLastSend = mil;
    }
    */
     
    curFile.read(buffer,FRAME_SIZE);
  
    showCurrentFrame();
  }

  void showBlackFrame()
  {
    for(int i=0;i<NUM_LEDS;i++) simpleSetLed(i, CRGB::Black);
    FastLED.show();
  }

  void showIdFrame()
  {
    if(groupID == -1 || localID == -1) return;
    setRange(0, 3, groupColor);
    setRange(NUM_LEDS-1-localID, NUM_LEDS-1, CHSV(localID * 255.0f / 12, 255, 255));
  }
    
  void showCurrentFrame()
  {
    
    for(int i=0;i<NUM_LEDS;i++)
    {
      //ARGB format
      byte r = buffer[i*4+2];
      byte g = buffer[i*4+1];
      byte b = buffer[i*4+0];
      simpleSetLed(i, CRGB(r,g,b));
  
     //buffer[i*4+3] /alpha
    }

    FastLED.show();
  }

  //Helpers
  long msToBytePos(long t) const { return msToFrame(t) * FRAME_SIZE; } //rgba
  long msToFrame(long timeMs) const { return timeMs*fps/1000; }
  long frameToMs(long frame) const { return frame*1000/fps; }
  float frameToSeconds(long frame) const { return frame*1.0f/fps; };
  float msToSeconds(long timeMs) const { return timeMs/1000.0f; }
  long secondsToMs(float s) const { return s * 1000; }
  long secondsToFrame(float s) const { return s*fps; }
  long bytePosToFrame(long pos) const { return pos / FRAME_SIZE; }
  long bytePosToMs(long pos) const { return frameToMs(bytePosToFrame(pos)); }
  long bytePosToSeconds (long pos) const { return frameToSeconds(bytePosToFrame(pos)); }
  
  //TIME COMMANDS
  void load(String path)
  {
    setFullColor(CRGB::Black);
    
    DBG("Load file "+path);
  
    DBG("Reading meta data");
    metaDataFile = FileManager::openFile(path+".meta", false); //false is for reading
    if(!metaDataFile)
    {
      DBG("Error reading metadata");
    }else
    {
      DeserializationError error = deserializeJson(metaData, metaDataFile);
       if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
      }else
      {
        fps = metaData["fps"];
        groupID = metaData["group"];
        localID = metaData["id"];

        groupColor = CRGB((float)(metaData["groupColor"][0])*255,
        (float)(metaData["groupColor"][1])*255,
        (float)(metaData["groupColor"][2])*255);
        
        Serial.println("Loaded meta, id "+String(groupID)+":"+String(localID)+" at "+String(fps)+" fps.");
      }

      metaDataFile.close();
    }

    DBG("Loading colors..");
    curFile = FileManager::openFile(path+".colors", false); //false is for reading
    if(!curFile)
    {
      DBG("Error playing file "+path);
    }else
    {
      long totalBytes = curFile.size();
      totalFrames = bytePosToFrame(totalBytes);
      totalTime = bytePosToSeconds(totalBytes);
      curTimeMs = 0;
      isPlaying = false;
      DBG("File loaded, "+String(totalBytes)+" bytes"+", "+String(totalFrames)+" frames, "+String(totalTime)+" time");
    }
  }

  void play(float atTime = 0)
  {
    DBG("Play "+String(atTime));
    if(!curFile) return;
    
    isPlaying = true;
    
    seek(atTime);
    prevTimeMs = millis();
  }

  void seek(float t)
  {
    if(!curFile) return;
    
    DBG("Seek to "+String(t));
    curTimeMs = secondsToMs(t);
    prevTimeMs = millis();
    
    curFile.seek(msToBytePos(max(curTimeMs, (long)0)));

    if(curTimeMs < 0)
    {
      showBlackFrame();
    }else if(!isPlaying)
    {
      curFile.read(buffer,FRAME_SIZE);
      showCurrentFrame();
    }
  }

  void pause()
  {
    DBG("Pause");
    isPlaying = false;
  }

  void stop()
  {
    DBG("Stop");
    isPlaying = false;
    setFullColor(CRGB::Black);
  }

  void togglePlayPause()
  {
    if(!isPlaying) pause();
    else play();
  }


  //OSC

  #if USE_OSC
  boolean handleMessage(OSCMessage &msg, int offset)
  {
    DBG("Bake player parse message");
    
    int pOffset = msg.match("/player", offset);
    if(pOffset == 0) return false;
    int newOffset = offset+pOffset;
    
    if(msg.match("/load",newOffset))
    {
      char filename[32];
      msg.getString(0,filename, 32);
      load(String(filename));

      idMode = false;
      if(msg.size() >= 2)
      {
        if(msg.isFloat(1)) idMode = msg.getFloat(1);
        else if(msg.isBoolean(1)) idMode = msg.getBoolean(1);
        else if(msg.isInt(1)) idMode = msg.getInt(1);
      }
    }else if(msg.match("/play",newOffset))
    {
      if(msg.size() >= 1 && msg.isString(0))
      {
        char filename[32];
        msg.getString(0,filename, 32);
        load(String(filename));
        play(msg.size() >= 2?msg.getFloat(1):-1);
      }else
      {
        play(msg.size() >= 1?msg.getFloat(0):-1);
      }
      idMode = false;
      
    }else if(msg.match("/pause",newOffset))
    {
      pause();
    }else if(msg.match("/resume",newOffset))
    {
      play();
    }else if(msg.match("/stop",newOffset))
    {
      stop();
    }else if(msg.match("/seek",newOffset))
    {
      timeToSeek = msg.getFloat(0);
    }
    else if(msg.match("/id",newOffset))
    {
      if(msg.size() >= 1)
      {
        if(msg.isFloat(0)) idMode = msg.getFloat(0);
        else if(msg.isBoolean(0)) idMode = msg.getBoolean(0);
        else if(msg.isInt(0)) idMode = msg.getInt(0);

        if(idMode)
        {
          showIdFrame();
          isPlaying = false;
        }
        else 
        {
          showBlackFrame();
        }
      }
    }
    else if(msg.match("/delete", newOffset))
    {
      char showName[32];
      msg.getString(0,showName, 32);
      FileManager::deleteFileIfExists(String(showName)+".colors");
      FileManager::deleteFileIfExists(String(showName)+".meta");
    }
    else
    {
      DBG("Bake player :: message not handled");
    }
    
    return true;
  }
  #endif
 
};

#endif
