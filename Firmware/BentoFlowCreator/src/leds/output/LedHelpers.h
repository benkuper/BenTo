#pragma once

#include "FastLED/FastLED.h"

class LedHelpers
{
public:
    static void clear(CRGB * leds, int numLeds);
    static void fillAll(CRGB * leds, int numLeds, CRGB c);
    static void fillRange(CRGB * leds, int numLeds, CRGB c, float start, float end, bool clear = true);
    static void point(CRGB * leds, int numLeds, CRGB c, float pos, float radius, bool clear = true);
};