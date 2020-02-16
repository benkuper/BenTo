#pragma once

#include "../../common/Common.h"
#include "FastLED/FastLED.h"

class RGBLedsManager
{
public:
    RGBLedsManager();

    CRGB leds;
    
    void init();
    void update();
};
    
