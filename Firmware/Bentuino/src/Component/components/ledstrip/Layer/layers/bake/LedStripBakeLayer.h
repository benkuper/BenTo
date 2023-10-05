#pragma once

class LedStripBakeLayer : public LedStripLayer
{
public:
    LedStripBakeLayer(LedStripComponent *strip) : LedStripLayer("bakeLayer", LedStripLayer::Bake, strip) {}
    ~LedStripBakeLayer() {}

    File curFile;
    File metaDataFile;
    StaticJsonDocument<200> metaData;

    int frameSize;

    // info
    int fps;
    float totalTime;
    long totalFrames;
    int groupID;
    int localID;
    Color groupColor;

    // control
    Parameter idMode{"idMode", false, var(), var(), true};
    Parameter loop{"loop", false, var(), var(), true};

    // playing
    bool isPlaying;
    long curTimeMs;
    long prevTimeMs;
    long timeSinceLastSeek;
    float timeToSeek; // used to limit seeking

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    bool playFrame();
    void showBlackFrame();
    void showIdFrame();
    void showCurrentFrame();

    // play control
    void load(String path);
    void play(float atTime = 0);
    void seek(float t, bool doSendEvent = true);
    void pause();
    void stop();
    void togglePlayPause();

    bool handleCommandInternal(const String &command, var *data, int numData) override;

    // Time computation helpers
    long msToBytePos(long t) const { return msToFrame(t) * frameSize; } // rgba
    long msToFrame(long timeMs) const { return timeMs * fps / 1000; }
    long frameToMs(long frame) const { return frame * 1000 / fps; }
    float frameToSeconds(long frame) const { return frame * 1.0f / fps; };
    float msToSeconds(long timeMs) const { return timeMs / 1000.0f; }
    long secondsToMs(float s) const { return s * 1000; }
    long secondsToFrame(float s) const { return s * fps; }
    long bytePosToFrame(long pos) const { return pos / frameSize; }
    long bytePosToMs(long pos) const { return frameToMs(bytePosToFrame(pos)); }
    long bytePosToSeconds(long pos) const { return frameToSeconds(bytePosToFrame(pos)); }

    DeclareComponentEventTypes(Loaded, LoadError, Playing, Paused, Stopped, Seek, Looped);
    DeclareComponentEventNames("Loaded", "LoadError", "Playing", "Paused", "Stopped", "Seek", "Looped");
};