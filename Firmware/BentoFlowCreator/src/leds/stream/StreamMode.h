#pragma once


#include "../LedMode.h"
#include <WiFiUdp.h>

#ifdef LED_COUNT
#define MAX_PACKET_SIZE 1000

class StreamMode : public LedMode {
    
public:
    StreamMode(CRGB * leds, int numLeds);
    ~StreamMode();

    WiFiUDP udp;

    //tempo
    const int receiveRate = 50; //receive at 60fps max
    long lastReceiveTime;

    uint8_t streamBuffer[MAX_PACKET_SIZE];
    int byteIndex;
    bool hasOverflowed;
    //uint8_t colorBuffer[3];
    //int colorBufferIndex;
    //int ledBufferIndex;

    void init() override;
    bool update() override;

    bool receiveUDP();

    void start() override;
    void stop() override;


    bool handleCommand(String command, var *data, int numData) override;
};

#endif