#pragma once


#include "../LedMode.h"
#include "../../files/FileManager.h"
#include <ArduinoJson.h>

#ifdef LED_COUNT

#define COLOR_SIZE 3
#define FRAME_SIZE (LED_COUNT * COLOR_SIZE)

class PlayerEvent
{
public:
  enum Type
  {
    Load,
    Play,
    Pause,
    Stop,
    Seek,
    Loop
  };
  PlayerEvent(Type t) : type(t) {}
  Type type;
};

class PlayerMode : public LedMode, public EventBroadcaster<PlayerEvent>
{

public:
  PlayerMode(CRGB *leds, int numLeds);
  ~PlayerMode();

  //file
#ifdef HAS_FILES
  File curFile;
  File metaDataFile;
#endif
  
  StaticJsonDocument<200> metaData;

  //info
  int fps;
  float totalTime;
  long totalFrames;
  int groupID;
  int localID;
  CRGB groupColor;

  //control
  bool idMode;
  bool loopShow;

  //playing
  bool isPlaying;
  long curTimeMs;
  long prevTimeMs;
  long timeSinceLastSeek;
  float timeToSeek; //used to limit seeking

  //byte buffer[FRAME_SIZE];

  void init() override;
  bool update() override;

  bool playFrame();
  void showBlackFrame();
  void showIdFrame();
  void showCurrentFrame();

  //void updateLeds();

  void start() override;
  void stop() override;

  //play control
  void load(String path);
  void play(float atTime = 0);
  void seek(float t, bool doSendEvent = true);
  void pause();
  void stopPlaying();
  void togglePlayPause();

  bool handleCommand(String command, var *data, int numData) override;

  //Time computation helpers
  #ifdef LED_COUNT
  long msToBytePos(long t) const { return msToFrame(t) * FRAME_SIZE; } //rgba
  long msToFrame(long timeMs) const { return timeMs * fps / 1000; }
  long frameToMs(long frame) const { return frame * 1000 / fps; }
  float frameToSeconds(long frame) const { return frame * 1.0f / fps; };
  float msToSeconds(long timeMs) const { return timeMs / 1000.0f; }
  long secondsToMs(float s) const { return s * 1000; }
  long secondsToFrame(float s) const { return s * fps; }
  long bytePosToFrame(long pos) const { return pos / FRAME_SIZE; }
  long bytePosToMs(long pos) const { return frameToMs(bytePosToFrame(pos)); }
  long bytePosToSeconds(long pos) const { return frameToSeconds(bytePosToFrame(pos)); }
  #endif
};

#endif //LED_COUNT