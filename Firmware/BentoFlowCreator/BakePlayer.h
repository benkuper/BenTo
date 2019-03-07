#ifndef BAKEPLAYER_H
#define BAKEPLAYER_H

#include "FileManager.h"
#include "FastLEDDefs.h"

#define FRAME_SIZE (NUM_LEDS*4)

class BakePlayer
{
public:

  File curFile;
  int fps = 100;
  byte buffer[FRAME_SIZE];

  bool isPlaying = false;
  
  long curTimeMs;
  long prevTimeMs;
  
  //File info
  float totalTime;
  long totalFrames;

  BakePlayer()
  {
    curTimeMs = 0;
    prevTimeMs = millis();
    
  }

  void update()
  {
    if(!curFile) return;
    if(!isPlaying) return;
    playFrame();
  }



  void playFrame()
  {
    if(curFile.available() < FRAME_SIZE)
    {
      DBG("End of show, available = "+String(curFile.available()));
      play(0);
    }

    curTimeMs += millis() - prevTimeMs; 
    
    prevTimeMs = millis();
    
    
    long pos = msToBytePos(curTimeMs);
    if(pos < curFile.position()) return; //waiting for frame

    while(curFile.position() > pos)
    {
      curFile.read(buffer,FRAME_SIZE);
      DBG("SKIPPED FRAME");
    }

    if(curFile.position() != pos)
    {
      DBG("SHOULD BE EXACTLY POS "+String(curFile.position())+", expected "+String(pos));
    }

    //DBG("Playing "+String(msToSeconds(curTimeMs))+"\t Frame : "+String(msToFrame(curTimeMs)));

    curFile.read(buffer,FRAME_SIZE);
    showCurrentFrame();
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
    DBG("Load file "+path);
    setFullColor(CRGB::Black);
    curFile = FileManager::openFile(path, false); //false is for reading
    if(!curFile)
    {
      DBG("Error playing file "+path);
    }else
    {
      long totalBytes = curFile.size();
      totalFrames = bytePosToFrame(totalBytes);
      totalTime = bytePosToSeconds(totalBytes);
      
      DBG("File loaded, "+String(totalBytes)+" bytes"+", "+String(totalFrames)+" frames, "+String(totalTime)+" time");
    }
  }

  void play(float atTime = -1)
  {
    DBG("Play "+String(atTime));
    if(!curFile) return;
    
    isPlaying = true;
    
    if(atTime >= 0)   seek(atTime);
    prevTimeMs = millis();
  }

  void seek(float t)
  {
    if(!curFile) return;
    
    DBG("Seek to "+String(t));
    curTimeMs = secondsToMs(t);
    prevTimeMs = millis();
    curFile.seek(msToBytePos(curTimeMs));
    
    if(!isPlaying)
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
      
    }else if(msg.match("/play",newOffset))
    {
      play(msg.size() > 0?msg.getFloat(0):-1);
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
      seek(msg.getFloat(0));
    }
    
    return true;
  }
  #endif
 
};

#endif
