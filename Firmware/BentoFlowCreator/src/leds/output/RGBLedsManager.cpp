#include "RGBLedsManager.h"

RGBLedsManager::RGBLedsManager() : Component("rgb"),
                                   globalBrightness(1)
#ifdef LED_USE_DMX
                                   ,
                                   timeSinceLastSend(0)
#endif
{
}

void RGBLedsManager::init()
{
#ifdef LED_COUNT
#ifdef LED_SEPARATE_CHANNELS
    for (int i = 0; i < LED_COUNT; i++)
    {
        const RGBLedPins l = rgbLedPins[i];
        if(l.rPin >= 0) pinMode(l.rPin, OUTPUT);
        if(l.gPin >= 0) pinMode(l.gPin, OUTPUT);
        if(l.bPin >= 0) pinMode(l.bPin, OUTPUT);
#ifdef ESP32
        int startChannel = i * 3;
        if(l.rPin >= 0) ledcSetup(startChannel, LED_PWM_FREQUENCY, LED_PWM_RESOLUTION);
        if(l.gPin >= 0) ledcSetup(startChannel + 1, LED_PWM_FREQUENCY, LED_PWM_RESOLUTION);
        if(l.bPin >= 0) ledcSetup(startChannel + 2, LED_PWM_FREQUENCY, LED_PWM_RESOLUTION);

        if(l.rPin >= 0) ledcAttachPin(l.rPin, startChannel);
        if(l.gPin >= 0) ledcAttachPin(l.gPin, startChannel + 1);
        if(l.bPin >= 0) ledcAttachPin(l.bPin, startChannel + 2);
#endif
    }
#elif defined LED_USE_DMX

    dmx.init();
#else
#ifdef LED_EN_PIN
    pinMode(LED_EN_PIN, OUTPUT); //enable LEDs
    digitalWrite(LED_EN_PIN, HIGH);
#endif
#if defined LED_CLK_PIN
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLK_PIN, LED_COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
#else
#ifdef LED_NUM_STRIPS
    #if LED_NUM_STRIPS >= 1 
    FastLED.addLeds<LED_TYPE, LED_PIN1, LED_COLOR_ORDER>(leds+LED_START1, LED_COUNT1).setCorrection(TypicalLEDStrip);
    
    #if LED_NUM_STRIPS >= 2 
    FastLED.addLeds<LED_TYPE, LED_PIN2, LED_COLOR_ORDER>(leds+LED_START2, LED_COUNT2).setCorrection(TypicalLEDStrip);
    
    #if LED_NUM_STRIPS >= 3 
    FastLED.addLeds<LED_TYPE, LED_PIN3, LED_COLOR_ORDER>(leds+LED_START3, LED_COUNT3).setCorrection(TypicalLEDStrip);
   
    #if LED_NUM_STRIPS >= 4 
    FastLED.addLeds<LED_TYPE, LED_PIN4, LED_COLOR_ORDER>(leds+LED_START4, LED_COUNT4).setCorrection(TypicalLEDStrip);
   
    #if LED_NUM_STRIPS >= 5 
    FastLED.addLeds<LED_TYPE, LED_PIN5, LED_COLOR_ORDER>(leds+LED_START5, LED_COUNT5).setCorrection(TypicalLEDStrip);
    

    #endif //5
    #endif //4
    #endif //3
    #endif //2
    #endif //1
#else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
#endif
#endif

#if defined LED2_TYPE
#if defined LED2_CLK_PIN
    FastLED.addLeds<LED2_TYPE, LED2_DATA_PIN, LED2_CLK_PIN, LED2_COLOR_ORDER>(leds, LED2_COUNT).setCorrection(TypicalLEDStrip);
#else
    FastLED.addLeds<LED2_TYPE, LED2_DATA_PIN, LED2_COLOR_ORDER>(leds, LED2_COUNT).setCorrection(TypicalLEDStrip);
#endif //LED2_CLK
#endif //LED2

#endif

#ifdef USE_PREFERENCES
    prefs.begin(name.c_str());
    setBrightness(prefs.getFloat("brightness", globalBrightness), false);
    prefs.end();
#elif defined USE_SETTINGS_MANAGER
    //init once with a json if it doesn't exist yet
    prefs.readSettings(String("/" + name + ".json").c_str());
    float brightness = prefs.getFloat("brightness", globalBrightness);
    prefs.loadJson(String("{\"brightness\":\"" + String(brightness) + "\"}").c_str());
    prefs.writeSettings(String("/" + name + ".json").c_str());

    //actually read the data
    prefs.readSettings(String("/" + name + ".json").c_str());
    setBrightness(prefs.getFloat("brightness", globalBrightness), false);
#endif

#endif //LED_COUNT
}

void RGBLedsManager::update()
{
#ifdef LED_COUNT
#ifdef LED_SEPARATE_CHANNELS
    for (int i = 0; i < LED_COUNT; i++)
    {
#ifndef PWMVAL
        int startChannel = i * 3;
        ledcWrite(startChannel, map(leds[i].r * globalBrightness, 0, 255, 0, 1023));
        ledcWrite(startChannel + 1, map(leds[i].g * globalBrightness, 0, 255, 0, 1023));
        ledcWrite(startChannel + 2, map(leds[i].b * globalBrightness, 0, 255, 0, 1023));
#else
        const RGBLedPins l = rgbLedPins[i];
        analogWrite(l.rPin, PWMVAL(leds[i].r * globalBrightness));
        analogWrite(l.gPin, PWMVAL(leds[i].g * globalBrightness));
        analogWrite(l.bPin, PWMVAL(leds[i].b * globalBrightness));
#endif
    }
#elif defined LED_USE_DMX

    long t = millis();
    if (t > timeSinceLastSend + updateBreakTime)
    {
        for (int i = 0; i < LED_COUNT; i++)
        {
            dmx.write(dmxStartChannels[i], leds[i].r * globalBrightness);
            dmx.write(dmxStartChannels[i] + 1, leds[i].g * globalBrightness);
            dmx.write(dmxStartChannels[i] + 2, leds[i].b * globalBrightness);
        }

        dmx.update();
        timeSinceLastSend = t;
    }

#else

FastLED.show();

#endif
#endif //LED_COUNT
}

void RGBLedsManager::setBrightness(float value, bool save)
{
    globalBrightness = min(max(value, 0.f), 1.f);

#ifdef LED_COUNT
#ifdef LED_SEPARATE_CHANNELS
#elif defined LED_USE_DMX
#else
    FastLED.setBrightness((int)(globalBrightness * 60));
    FastLED.show();
#endif
    if (save)
    {
#ifdef USE_PREFERENCES
        prefs.begin(name.c_str());
        prefs.putFloat("brightness", globalBrightness);
        prefs.end();
#elif defined USE_SETTINGS_MANAGER
        prefs.readSettings(String("/" + name + ".json").c_str());
        prefs.setFloat("brightness", globalBrightness);
#endif
    }
#endif //LED_COUNT
}

bool RGBLedsManager::handleCommand(String command, var *data, int numData)
{
#ifdef LED_COUNT
    if (checkCommand(command, "brightness", numData, 1))
    {
        setBrightness(data[0].floatValue());
        return true;
    }
    else if (checkCommand(command, "fill", numData, 3))
    {
        CRGB c((int)(data[0].floatValue() * 255), (int)(data[1].floatValue() * 255), (int)(data[2].floatValue() * 255));
        if (numData >= 4)
            c.nscale8((int)(data[3].floatValue() * 255));
        sendEvent(RGBLedsEvent(RGBLedsEvent::ASK_FOCUS));
        fillAll(c);
        return true;
    }
    else if (checkCommand(command, "range", numData, 5))
    {
        bool hasAlpha = numData >= 6;
        CRGB c((int)(data[0].floatValue() * 255), (int)(data[1].floatValue() * 255), (int)(data[2].floatValue() * 255));
        if (hasAlpha)
            c.nscale8((int)(data[3].floatValue() * 255));
        fillRange(c, data[hasAlpha ? 4 : 3].floatValue(), data[hasAlpha ? 5 : 4].floatValue());
        sendEvent(RGBLedsEvent(RGBLedsEvent::ASK_FOCUS));
        return true;
    }
    else if (checkCommand(command, "point", numData, 5))
    {
        bool hasAlpha = numData >= 6;
        CRGB c((int)(data[0].floatValue() * 255), (int)(data[1].floatValue() * 255), (int)(data[2].floatValue() * 255));
        if (hasAlpha)
            c.nscale8((int)(data[3].floatValue() * 255));
        point(c, data[hasAlpha ? 4 : 3].floatValue(), data[hasAlpha ? 5 : 4].floatValue());
        sendEvent(RGBLedsEvent(RGBLedsEvent::ASK_FOCUS));
        return true;
    }
#endif

    return false;
}

//Helpers
void RGBLedsManager::clear()
{
#ifdef LED_COUNT
#if defined LED_SEPARATE_CHANNELS || defined LED_USE_DMX
    for (int i = 0; i < LED_COUNT; i++)
        leds[i] = CRGB::Black;
#else
    FastLED.clear();
#endif
#endif //LED_COUNT
}

void RGBLedsManager::fillAll(CRGB c)
{
    fillRange(c, 0, 1);
}

void RGBLedsManager::fillRange(CRGB c, float start, float end, bool doClear)
{
#ifdef LED_COUNT
    if (doClear)
        clear();
    LedHelpers::fillRange(leds, LED_COUNT, c, start, end);
#endif
}

void RGBLedsManager::point(CRGB c, float pos, float fade, bool doClear)
{
#ifdef LED_COUNT
    if (doClear)
        clear();
    LedHelpers::point(leds, LED_COUNT, c, pos, fade);
#endif
}

void RGBLedsManager::setLed(int index, CRGB c)
{
#ifdef LED_COUNT

    if (index < 0 || index >= LED_COUNT)
        return;
    leds[index] = c;
#endif
}
