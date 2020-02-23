#pragma once

#include "../LedMode.h"
#include <WiFiUdp.h>

class StreamMode : public LedMode {
    
public:
    StreamMode(CRGB * leds, int numLeds);
    ~StreamMode();

    WiFiUDP udp;

    //tempo
    const int receiveRate = 60; //receive at 60fps max
    long lastReceiveTime;

    CRGB * ledBuffer;
    uint8_t colorBuffer[3];
    int colorBufferIndex;
    int ledBufferIndex;

    void init() override;
    void update() override;

    void receiveUDP();
    void processBuffer();

    void start() override;
    void stop() override;


    bool handleCommand(String command, var *data, int numData) override;
};