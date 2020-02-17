#include "RGBLedsManager.h"

RGBLedsManager::RGBLedsManager() : Component("rgb"),
                                   globalBrightness(1)
{
}

void RGBLedsManager::init()
{
    pinMode(LED_EN_PIN, OUTPUT); //enable LEDs
    digitalWrite(LED_EN_PIN, HIGH);

    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLK_PIN, LED_COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
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
        CRGB c((int)(data[0].floatValue()*255), (int)(data[1].floatValue()*255), (int)(data[2].floatValue()*255));
        if(numData >= 4) c.nscale8((int)(data[3].floatValue() * 255));
        fillAll(c);
        return true;
    }else if(checkCommand(command, "range", numData, 5))
    {
        bool hasAlpha = numData >= 6;
        CRGB c((int)(data[0].floatValue()*255), (int)(data[1].floatValue()*255), (int)(data[2].floatValue()*255));
        if(hasAlpha) c.nscale8((int)(data[3].floatValue() * 255));
        fillRange(c, data[hasAlpha?4:3].floatValue(), data[hasAlpha?5:4].floatValue());
        return true;
    }else if(checkCommand(command, "point", numData, 5))
    {
        bool hasAlpha = numData >= 6;
        CRGB c((int)(data[0].floatValue()*255), (int)(data[1].floatValue()*255), (int)(data[2].floatValue()*255));
        if(hasAlpha) c.nscale8((int)(data[3].floatValue() * 255));
        point(c, data[hasAlpha?4:3].floatValue(), data[hasAlpha?5:4].floatValue());
        return true;
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
    LedHelpers::fillRange(leds, LED_COUNT, c, start, end);
}

void RGBLedsManager::point(CRGB c, float pos, float fade, bool doClear)
{
    if (doClear) clear();
    LedHelpers::point(leds, LED_COUNT, c, pos, fade);
}

void RGBLedsManager::setLed(int index, CRGB c) 
{
    if(index < 0 || index >= LED_COUNT) return;
    leds[index] = c;
}
