#include "RGBLedsManager.h"

RGBLedsManager::RGBLedsManager() : Component("rgb"),
                                   globalBrightness(1)
{
}

void RGBLedsManager::init()
{
    pinMode(LEDS_ENABLE_PIN, OUTPUT); //enable LEDs
    digitalWrite(LEDS_ENABLE_PIN, HIGH);

    FastLED.addLeds<LEDS_TYPE, LEDS_DATA_PIN, LEDS_CLK_PIN, LEDS_COLOR_ORDER>(leds, LEDS_COUNT).setCorrection(TypicalLEDStrip);
    prefs.begin(name.c_str(), true);
    setBrightness(prefs.getFloat("brightness", globalBrightness), false);
    prefs.end();
}

void RGBLedsManager::update()
{
    FastLED.show();
}

void RGBLedsManager::setBrightness(float value, bool save)
{
    globalBrightness = min(max(value, 0.f), 1.f);
    FastLED.setBrightness((int)(globalBrightness * 60));
    FastLED.show();

    if (save)
    {
        prefs.begin(name.c_str());
        globalBrightness = prefs.getFloat("brightness", globalBrightness);
        prefs.end();
    }
}

bool RGBLedsManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "brightness", numData, 1))
    {
        setBrightness(data[0].floatValue());
        return true;
    }else if(checkCommand(command, "fill", numData, 3))
    {
        fillAll(CRGB(data[0].floatValue()*255, data[1].floatValue()*255, data[2].floatValue()*255));
    }else if(checkCommand(command, "range", numData, 5))
    {
        fillRange(CRGB(data[0].floatValue()*255, data[1].floatValue()*255, data[2].floatValue()*255), data[3].floatValue(), data[4].floatValue());
    }else if(checkCommand(command, "point", numData, 5))
    {
        point(CRGB(data[0].floatValue()*255, data[1].floatValue()*255, data[2].floatValue()*255), data[3].floatValue(), data[4].floatValue());
    }

    return false;
}


//Helpers
void RGBLedsManager::clear()
{
    FastLED.clear();
}

void RGBLedsManager::fillAll(CRGB c)
{
    fillRange(c, 0 , 1);
}

void RGBLedsManager::fillRange(CRGB c, float start, float end, bool doClear)
{
    if (doClear) clear();
    LedHelpers::fillRange(leds, LEDS_COUNT, c, start, end);
}

void RGBLedsManager::point(CRGB c, float pos, float fade, bool doClear)
{
    if (doClear) clear();
    LedHelpers::point(leds, LEDS_COUNT, c, pos, fade);
}
