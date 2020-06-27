
#include "PlayerMode.h"

#ifdef LED_COUNT

PlayerMode::PlayerMode(CRGB *leds, int numLeds) : LedMode("player", leds, numLeds),
                                                  fps(0),
                                                  totalTime(0),
                                                  totalFrames(0),
                                                  groupID(-1),
                                                  localID(-1),
                                                  idMode(false),
                                                  loopShow(false),
                                                  isPlaying(false),
                                                  curTimeMs(0),
                                                  prevTimeMs(0),
                                                  timeSinceLastSeek(0),
                                                  timeToSeek(0)
{
}

PlayerMode::~PlayerMode()
{
  //free(ledBuffer);
}

void PlayerMode::init()
{
}

bool PlayerMode::update()
{
#ifdef HAS_FILES
  if (!curFile)
    return false;

  if (idMode)
  {
    showIdFrame();
    return true;
  }

  if (timeToSeek != -1 && millis() > timeSinceLastSeek + 20)
  {
    seek(timeToSeek);
    timeToSeek = -1;
    timeSinceLastSeek = millis();
  }

  if (!isPlaying)
  {
    showBlackFrame();
    return true;
  }

  return playFrame();
#else
  showBlackFrame();
  return true;
#endif
}

bool PlayerMode::playFrame()
{
#ifdef HAS_FILES
  if (curFile.available() < FRAME_SIZE)
  {
    DBG("End of show");
    if (loopShow)
    {
      DBG("Loop");
      sendEvent(PlayerEvent(PlayerEvent::Loop));
      play(0);
    }
    else
    {
      isPlaying = false;
      return false;
    }
  }

  long mil = millis();
  curTimeMs += mil - prevTimeMs;
  prevTimeMs = mil;

  long fPos = curFile.position();
  long pos = msToBytePos(curTimeMs);

  if (pos < 0)
    return false;
  if (pos < fPos)
    return false; //waiting for frame

  int skippedFrames = 0;
  while (fPos < pos)
  {
    skippedFrames++;
    curFile.read((uint8_t *)leds, FRAME_SIZE);
    fPos = curFile.position();
    if (curFile.available() < FRAME_SIZE)
    {
      DBG("Player overflowed, should not be here");
      return false;
    } 
  }

  if(skippedFrames > 0)
  {
    DBG("Skipped frame " + String(skippedFrames));
  }

  if (fPos != pos)
  {
    DBG("Error, position is " + String(fPos) + ", expected " + String(pos));
  }

  curFile.read((uint8_t *)leds, FRAME_SIZE);

  //showCurrentFrame();
#endif

  return true;
}

void PlayerMode::showBlackFrame()
{
  LedHelpers::clear(leds, numLeds);
}

void PlayerMode::showIdFrame()
{
  if (groupID == -1 || localID == -1)
    return;
  LedHelpers::fillRange(leds, numLeds, groupColor, .9f, 1);
  CRGB c = rgb2hsv_approximate(CHSV(localID * 255.0f / 12, 255, 255));
  LedHelpers::fillRange(leds, numLeds, c, 0, localID * 1.f / numLeds, false);
  //updateLeds();
}

/*
void PlayerMode::showCurrentFrame()
{
  for (int i = 0; i < numLeds; i++)
  {
    leds[LEDMAP(i)] = CRGB(buffer[i * COLOR_SIZE + 2], buffer[i * COLOR_SIZE + 1], buffer[i * COLOR_SIZE + 0]);
  }
  updateLeds();
}
*/

void PlayerMode::start()
{
  LedMode::start();
}

void PlayerMode::stop()
{
  LedMode::stop();
  stopPlaying();
}

void PlayerMode::load(String path)
{
  showBlackFrame();

#ifdef HAS_FILES
  DBG("Load file " + path);
  DBG("Reading meta data");
  metaDataFile = FileManager::openFile(path + ".meta", false); //false is for reading
  if (!metaDataFile)
  {
    DBG("Error reading metadata");
  }
  else
  {
    DeserializationError error = deserializeJson(metaData, metaDataFile);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
    }
    else
    {
      fps = metaData["fps"];
      groupID = metaData["group"];
      localID = metaData["id"];

      groupColor = CRGB((float)(metaData["groupColor"][0]) * 255,
                        (float)(metaData["groupColor"][1]) * 255,
                        (float)(metaData["groupColor"][2]) * 255);

      Serial.println("Loaded meta, id " + String(groupID) + ":" + String(localID) + " at " + String(fps) + " fps.");
    }

    metaDataFile.close();
  }

  DBG("Loading colors..");
  curFile = FileManager::openFile(path + ".colors", false); //false is for reading
  if (!curFile)
  {
    DBG("Error playing file " + path);
  }
  else
  {
    long totalBytes = curFile.size();
    totalFrames = bytePosToFrame(totalBytes);
    totalTime = bytePosToSeconds(totalBytes);
    curTimeMs = 0;
    isPlaying = false;
    DBG("File loaded, " + String(totalBytes) + " bytes" + ", " + String(totalFrames) + " frames, " + String(totalTime) + " time");
  }

  sendEvent(PlayerEvent(PlayerEvent::Play));
#endif
}

void PlayerMode::play(float atTime)
{
#ifdef HAS_FILES
  DBG("Play " + String(atTime));
  if (!curFile)
    return;

  isPlaying = true;

  seek(atTime, false);
  prevTimeMs = millis();

  sendEvent(PlayerEvent(PlayerEvent::Play));
#endif
}

void PlayerMode::seek(float t, bool doSendEvent)
{
#ifdef HAS_FILES
  if (!curFile)
    return;

  DBG("Seek to " + String(t));
  curTimeMs = secondsToMs(t);
  prevTimeMs = millis();

  curFile.seek(msToBytePos(max(curTimeMs, (long)0)));

  if (curTimeMs < 0)
  {
    showBlackFrame();
  }
  else if (!isPlaying)
  {
    curFile.read((uint8_t*)leds, FRAME_SIZE);
    //showCurrentFrame();
  }

  if (doSendEvent)
    sendEvent(PlayerEvent(PlayerEvent::Play));
#endif
}

void PlayerMode::pause()
{
#ifdef HAS_FILES
  DBG("Pause");
  isPlaying = false;
  sendEvent(PlayerEvent(PlayerEvent::Pause));
#endif
}

void PlayerMode::stopPlaying()
{
#ifdef HAS_FILES
  DBG("Stop");
  isPlaying = false;
  showBlackFrame();
  sendEvent(PlayerEvent(PlayerEvent::Stop));
#endif
}

void PlayerMode::togglePlayPause()
{
  if (!isPlaying)
    pause();
  else
    play();
}

bool PlayerMode::handleCommand(String command, var *data, int numData)
{
  if (checkCommand(command, "load", numData, 1))
  {
    load(data[0].stringValue());
    idMode = numData > 1 ? data[1].intValue() : false;
    return true;
  }

  if (checkCommand(command, "play", numData, 0))
  {
    if (numData > 0 && data[0].type == 's')
    {
      load(data[0].stringValue());
      play(numData > 1 ? data[1].floatValue() : 0);
    }
    else
    {
      play(numData > 0 ? data[0].floatValue() : 0);
    }

    if (numData > 1)
    {
      loopShow = data[1].intValue() == 1;
    }

    idMode = false;
    return true;
  }

  if (checkCommand(command, "pause", numData, 0))
  {
    pause();
    return true;
  }

  if (checkCommand(command, "resume", numData, 0))
  {
    play();
    return true;
  }

  if (checkCommand(command, "stop", numData, 0))
  {
    stopPlaying();
    return true;
  }

  if (checkCommand(command, "seek", numData, 1))
  {
    timeToSeek = data[0].floatValue();
    return true;
  }

  if (checkCommand(command, "id", numData, 1))
  {
    idMode = data[0].intValue();
    NDBG("ID Mode " + String(idMode));

    if (idMode)
    {
      showIdFrame();
      isPlaying = false;
    }
    else
    {
      showBlackFrame();
    }
    return true;
  }

  if (checkCommand(command, "delete", numData, 1))
  {
#ifdef HAS_FILES
    FileManager::deleteFileIfExists(String(data[0].stringValue()) + ".colors");
    FileManager::deleteFileIfExists(String(data[0].stringValue()) + ".meta");
#endif
    return true;
  }

  return true;
}

#endif