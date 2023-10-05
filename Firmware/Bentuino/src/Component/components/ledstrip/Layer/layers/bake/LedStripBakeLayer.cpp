bool LedStripBakeLayer::initInternal(JsonObject o)
{
    LedStripLayer::initInternal(o);

    frameSize = numLeds * 4;

    fps = 0;
    totalTime = 0;
    totalFrames = 0;
    groupID = -1;
    localID = -1;
    AddAndSetParameter(idMode);
    AddAndSetParameter(loop);
    isPlaying = false;
    curTimeMs = 0;
    prevTimeMs = 0;
    timeSinceLastSeek = 0;
    timeToSeek = 0;

    return true;
}

void LedStripBakeLayer::updateInternal()
{
    if (!curFile)
        return;

    if (idMode.boolValue())
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
    {
        showBlackFrame();
        return;
    }

    playFrame();
}

void LedStripBakeLayer::clearInternal()
{
}

bool LedStripBakeLayer::playFrame()
{
    if (curFile.available() < frameSize)
    {
        NDBG("End of show");
        if (loop.boolValue())
        {
            sendEvent(Looped);
            play(0);
        }
        else
        {
            stop();
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
        return false; // waiting for frame

    int skippedFrames = 0;
    while (fPos < pos)
    {
        skippedFrames++;
        curFile.read((uint8_t *)colors, frameSize);
        fPos = curFile.position();
        if (curFile.available() < frameSize)
        {
            DBG("Player overflowed, should not be here");
            return false;
        }
    }

    if (skippedFrames > 0)
    {
        DBG("Skipped frame " + String(skippedFrames));
    }

    if (fPos != pos)
    {
        DBG("Error, position is " + String(fPos) + ", expected " + String(pos));
    }

    curFile.read((uint8_t *)colors, frameSize);

    // showCurrentFrame();
    return true;
}

void LedStripBakeLayer::showBlackFrame()
{
    clearColors();
}

void LedStripBakeLayer::showIdFrame()
{
    if (groupID == -1 || localID == -1)
        return;
    fillRange(groupColor, .9f, 1);
    Color c = Color::HSV(localID / 12.f, 1, 1);
    fillRange(c, 0, localID * 1.f / numLeds, false);
}

void LedStripBakeLayer::load(String path)
{
    showBlackFrame();

    NDBG("Load file " + path);
    NDBG("Reading meta data");
    metaDataFile = FilesComponent::instance->openFile(path + ".meta", false); // false is for reading
    if (!metaDataFile)
    {
        NDBG("Error reading metadata");
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

            groupColor = Color((float)(metaData["groupColor"][0]) * 255,
                               (float)(metaData["groupColor"][1]) * 255,
                               (float)(metaData["groupColor"][2]) * 255);

            Serial.println("Loaded meta, id " + String(groupID) + ":" + String(localID) + " at " + String(fps) + " fps.");
        }

        metaDataFile.close();
    }

    NDBG("Loading colors..");
    curFile = FilesComponent::instance->openFile(path + ".colors", false); // false is for reading
    if (!curFile)
    {
        NDBG("Error playing file " + path);
        sendEvent(LoadError);
    }
    else
    {
        long totalBytes = curFile.size();
        totalFrames = bytePosToFrame(totalBytes);
        totalTime = bytePosToSeconds(totalBytes);
        curTimeMs = 0;
        isPlaying = false;
        NDBG("File loaded, " + String(totalBytes) + " bytes" + ", " + String(totalFrames) + " frames, " + String(totalTime) + " time");
    }

    play(0);
}

void LedStripBakeLayer::play(float atTime)
{
    DBG("Play " + String(atTime));
    if (!curFile)
        return;

    isPlaying = true;

    seek(atTime, false);
    prevTimeMs = millis();

    sendEvent(Playing);
}

void LedStripBakeLayer::seek(float t, bool doSendEvent)
{
    if (!curFile)
        return;

    NDBG("Seek to " + String(t));
    curTimeMs = secondsToMs(t);
    prevTimeMs = millis();

    curFile.seek(msToBytePos(max(curTimeMs, (long)0)));

    if (curTimeMs < 0)
    {
        showBlackFrame();
    }
    else if (!isPlaying)
    {
        curFile.read((uint8_t *)colors, frameSize);
    }

    if (doSendEvent)
        sendEvent(Seek);
}

void LedStripBakeLayer::pause()
{
    DBG("Pause");
    isPlaying = false;
    sendEvent(Paused);
}

void LedStripBakeLayer::stop()
{
    DBG("Stop");
    isPlaying = false;
    showBlackFrame();
    sendEvent(Stopped);
}

void LedStripBakeLayer::togglePlayPause()
{
    if (!isPlaying)
        pause();
    else
        play();
}

bool LedStripBakeLayer::handleCommandInternal(const String &command, var *data, int numData)
{
    if (checkCommand(command, "load", numData, 1))
    {
        load(data[0].stringValue());
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
        stop();
        return true;
    }

    if (checkCommand(command, "seek", numData, 1))
    {
        timeToSeek = data[0].floatValue();
        return true;
    }
    return true;
}