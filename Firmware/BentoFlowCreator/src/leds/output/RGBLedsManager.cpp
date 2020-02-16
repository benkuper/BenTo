#include "RGBLedsManager.h"


RGBLedsManager::RGBLedsManager() : 
    Component("rgb"),
    globalBrightness(.1f)
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

    NDBG("Leds manager init");
    fillColor(CRGB::Red);
}

void RGBLedsManager::update()
{
    
}

void RGBLedsManager::updateLeds()
{
    FastLED.show();
}

void RGBLedsManager::fillColor(CRGB c)
{
    for(int i=0;i<LEDS_COUNT;i++) leds[i] = c;
    FastLED.show();
}

void RGBLedsManager::setBrightness(float value, bool save)
{
    globalBrightness = min(max(value, 0.f), 1.f);
    NDBG("Set brightness "+String(globalBrightness)+" / "+String((int)(globalBrightness*60)));
    
    FastLED.setBrightness((int)(globalBrightness*60));
    FastLED.show();

    if(save)
    {
        prefs.begin(name.c_str());
        globalBrightness = prefs.getFloat("brightness", globalBrightness);
        prefs.end();
    }
}

 bool RGBLedsManager::handleCommand(String command, var * data, int numData)
 {
     if(checkCommand(command, "fill", numData, 3))
     {
        fillColor(CRGB((int)(data[0].floatValue()*255),(int)(data[1].floatValue()*255), (int)(data[2].floatValue()*255)));
        return true;
     }else if(checkCommand(command,"brightness",numData,1))
     {
         setBrightness(data[0].floatValue());
         return true;
     }
     
     return false;
 }
