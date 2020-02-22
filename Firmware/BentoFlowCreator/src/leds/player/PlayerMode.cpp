
#include "PlayerMode.h"

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
  ledBuffer = (CRGB *)malloc(numLeds * sizeof(CRGB));
}

PlayerMode::~PlayerMode()
{
  free(ledBuffer);
}

void PlayerMode::init()
{
}

void PlayerMode::update()
{
  if (!curFile)
    return;

  if (idMode)
  {
    showIdFrame();
    return;
  }

  if (timeToSeek != -1 && millis() > timeSinceLastSeek + 20)
  {
    seek(timeToSeek);
    timeToSeek = -1;
    timeSinceLastSeek = millis();
  }

  if (!isPlaying)
    return;
  playFrame();
}

void PlayerMode::playFrame()
{
  if (curFile.available() < FRAME_SIZE)
  {
    DBG("End of show");
    if (loopShow)
    {
      DBG("Loop");
      sendEvent(PlayerEvent(PlayerEvent::Loop));
      play(0);
    }else
    {
      isPlaying = false;
      return;
    }
  }

  long mil = millis();
  curTimeMs += mil - prevTimeMs;
  prevTimeMs = mil;

  long fPos = curFile.position();
  long pos = msToBytePos(curTimeMs);

  if (pos < 0)
    return;
  if (pos < fPos)
    return; //waiting for frame

  while (fPos > pos)
  {
    curFile.read(buffer, FRAME_SIZE);
    DBG("Skipped frame");
  }

  if (fPos != pos)
  {
    DBG("Error, position is " + String(curFile.position()) + ", expected " + String(pos));
  }

  curFile.read(buffer, FRAME_SIZE);

  showCurrentFrame();
}

void PlayerMode::showBlackFrame()
{
  LedHelpers::clear(ledBuffer, numLeds);
  updateLeds();
}

void PlayerMode::showIdFrame()
{
  if (groupID == -1 || localID == -1)
    return;
  LedHelpers::fillRange(ledBuffer, numLeds, groupColor, 0, .1f);
  CRGB c =  rgb2hsv_approximate(CHSV(localID * 255.0f / 12, 255, 255));
  LedHelpers::fillRange(ledBuffer, numLeds, c, (numLeds - localID) * 1.f / numLeds, 1, false);
  updateLeds();
}

void PlayerMode::showCurrentFrame()
{
  for (int i = 0; i < numLeds; i++)
  {
    ledBuffer[i] = CRGB(buffer[i * 4 + 2], buffer[i * 4 + 1], buffer[i * 4 + 0]);
  }
  updateLeds();
}

void PlayerMode::updateLeds()
{
  if (!isActive)
    return;
  
  memcpy(leds, ledBuffer, numLeds * sizeof(CRGB));

}

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
}

void PlayerMode::play(float atTime)
{
  DBG("Play " + String(atTime));
  if (!curFile)
    return;

  isPlaying = true;

  seek(atTime, false);
  prevTimeMs = millis();

  sendEvent(PlayerEvent(PlayerEvent::Play));
}

void PlayerMode::seek(float t, bool doSendEvent)
{
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
    curFile.read(buffer, FRAME_SIZE);
    showCurrentFrame();
  }

  if (doSendEvent)
    sendEvent(PlayerEvent(PlayerEvent::Play));
}

void PlayerMode::pause()
{
  DBG("Pause");
  isPlaying = false;
  sendEvent(PlayerEvent(PlayerEvent::Pause));
}

void PlayerMode::stopPlaying()
{
  DBG("Stop");
  isPlaying = false;
  showBlackFrame();
  sendEvent(PlayerEvent(PlayerEvent::Stop));
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

    if(numData > 1)
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
    NDBG("ID Mode "+String(idMode));

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
    FileManager::deleteFileIfExists(String(data[0].stringValue()) + ".colors");
    FileManager::deleteFileIfExists(String(data[0].stringValue()) + ".meta");
    return true;
  }

  return true;
}
