#pragma once
#include "../common/Common.h"
#include "output/LedHelpers.h"

class LedMode : public Component
{
public:
    LedMode(const String &name, CRGB * leds, int numLeds) : Component(name), isActive(false), leds(leds), numLeds(numLeds)  {}
    virtual ~LedMode() {}

    bool isActive;

    CRGB * leds;
    int numLeds;

    virtual void init() {}
    virtual bool update() { return false; }

    virtual void start() { NDBG("Start"); isActive = true; }
    virtual void stop() { NDBG("Stop"); isActive = false; }
};