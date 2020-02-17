#pragma once

#include "../LedMode.h"
#include "../../files/FileManager.h"

#define FRAME_SIZE (LED_COUNT*4)

class PlayerMode : public LedMode {
    
public:
    PlayerMode(CRGB * leds, int numLeds);
    ~PlayerMode();

    File curFile;
    File metaDataFile;
    
    int fps;
    byte buffer[FRAME_SIZE];

    int groupID;
    int localID;
    
    CRGB groupColor;
    //StaticJsonDocument<200> metaData;

    void init() override;
    void update() override;

    void start() override;
    void stop() override;

    bool handleCommand(String command, var *data, int numData) override;
};