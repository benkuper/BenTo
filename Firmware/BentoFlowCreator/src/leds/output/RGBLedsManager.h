#pragma once

#include "../../common/Common.h"
#include "LedHelpers.h"

//To move to custom board definition
#define LEDS_COUNT 32
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

    void init();
    void update();


    void setBrightness(float value, bool save = false);
    bool handleCommand(String command, var * data, int numData) override;

    //Helpers
    void clear();
    void fillAll(CRGB c);
    void fillRange(CRGB c, float start, float end, bool clear = true);
    void point(CRGB c, float pos, float radius, bool clear = true);
        
private:
    Preferences prefs;
};
    
