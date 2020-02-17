#pragma once
#include "../common/Common.h"
#include "output/LedHelpers.h"

class LedMode : public Component
{
public:
    LedMode(const String &name, CRGB * leds, int numLeds) : Component(name), leds(leds), numLeds(numLeds)  {}
    virtual ~LedMode() {}

    CRGB * leds;
    int numLeds;

    virtual void init() {}
    virtual void update() {}

    virtual void start() {}
    virtual void stop() {}
};