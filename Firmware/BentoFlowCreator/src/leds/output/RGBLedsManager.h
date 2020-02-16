#pragma once

#include "../../common/Common.h"
#include "FastLED/FastLED.h"

//To move to custom board definition
#define LEDS_COUNT      32
#define LEDS_ENABLE_PIN 26
#define LEDS_DATA_PIN   5
#define LEDS_CLK_PIN    18
#define LEDS_TYPE       SK9822
#define LEDS_COLOR_ORDER BGR

class RGBLedsManager :
    public Component
{
public:
    RGBLedsManager();

    CRGB leds[LEDS_COUNT];
    float globalBrightness;
    Preferences prefs;

    void init();
    void update();
    void updateLeds();

    void fillColor(CRGB c);

    void setBrightness(float value, bool save = false);

    bool handleCommand(String command, var * data, int numData) override;
};
    
