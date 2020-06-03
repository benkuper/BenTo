#pragma once

#ifdef LED_SEPARATE_CHANNELS
#elif defined LED_USE_DMX
#ifdef ESP8266
#include "DMX/ESPDMX.h"
#elif defined ESP32
#include "DMX/SparkfunDMX.h"
#endif // 8266/32
#else
#endif
#include "FastLED/FastLED.h"

class LedHelpers
{
public:
    static void clear(CRGB * leds, int numLeds);
    static void fillAll(CRGB * leds, int numLeds, CRGB c);
    static void fillRange(CRGB * leds, int numLeds, CRGB c, float start, float end, bool clear = true);
    static void point(CRGB * leds, int numLeds, CRGB c, float pos, float radius, bool clear = true);
};