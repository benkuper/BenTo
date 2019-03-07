#ifndef BAKEPLAYER_H
#define BAKEPLAYER_H

#include "FileManager.h"
#include "FastLEDDefs.h"

#define BUFFER_LENGTH NUM_LEDS*4

class BakePlayer
{
public:

  File curFile;
  long timeAtPlay = 0;
  bool isPausing;
  int fps = 50;
  byte buffer[BUFFER_LENGTH];
  
  BakePlayer()
  {
    isPausing = false;
  }

  void update()
  {
    if(!curFile) return;
    
    if(curFile.available() < BUFFER_LENGTH)
    {
      DBG("End of show, available = "+String(curFile.available()));
      resetTime();
    }

    if(isPausing) 
    {
      timeAtPlay = millis();
      return;
    }
    
    long curTime = millis()-timeAtPlay;
    long pos = getBytePosForTime(curTime);

     if(pos <= curFile.position()) return; //waiting for frame
    
    int bytesRead = curFile.read(buffer,BUFFER_LENGTH);

    DBG("Bake curTime "+String(curTime)+"\tpos : "+String(pos)+"\tfilePos : "+String(curFile.position())+"\tread "+String(bytesRead));
   
    for(int i=0;i<NUM_LEDS;i++)
    {
      byte r = buffer[i*4+2];
      byte g = buffer[i*4+1];
      byte b = buffer[i*4+0];
      simpleSetLed(i, CRGB(r,g,b));
      if(i == 0)
      {
        DBG("Col 1"+String(r)+","+String(g)+","+String(b));
      }
     //buffer[i*4+3] /alpha
    }

    FastLED.show();
  }

  long getBytePosForTime(long t)
  {
    int curFrame = t*fps/1000;
    return curFrame * NUM_LEDS * 4; //rgba
  }

  void playFile(String path)
  {
    DBG("Play file "+path);
    setFullColor(CRGB::Black);
    curFile = FileManager::openFile(path, false); //false is for reading
    if(!curFile)
    {
      DBG("Error playing file "+path);
    }else
    {
      DBG("File has "+String(curFile.size())+" bytes");
    }
    resetTime();
  }

  void togglePlayPause()
  {
    isPausing = !isPausing;
  }

  void resetTime()
  {
    timeAtPlay = millis();
    curFile.seek(0);
  }
  
};

#endif
