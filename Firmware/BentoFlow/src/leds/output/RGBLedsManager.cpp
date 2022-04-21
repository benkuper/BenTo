#include "RGBLedsManager.h"

const String RGBLedsEvent::eventNames[RGBLedsEvent::TYPES_MAX]{"askFocus", "brightnessStatus"};

RGBLedsManager::RGBLedsManager() : Component("rgb"),
                                   globalBrightness(1),
                                   ledEnabled(false)
#ifdef LED_USE_DMX
                                   ,
                                   timeSinceLastSend(0)
#endif
{
#ifdef LED_DEFAULT_BRIGHTNESS
    globalBrightness = LED_DEFAULT_BRIGHTNESS;
#endif
}

void RGBLedsManager::init()
{
#ifdef LED_COUNT
#ifdef LED_SEPARATE_CHANNELS
    for (int i = 0; i < LED_COUNT; i++)
    {
        const RGBLedPins l = rgbLedPins[i];
        if (l.rPin >= 0)
            pinMode(l.rPin, OUTPUT);
        if (l.gPin >= 0)
            pinMode(l.gPin, OUTPUT);
        if (l.bPin >= 0)
            pinMode(l.bPin, OUTPUT);
#ifdef ESP32
        int startChannel = i * 3;
        if (l.rPin >= 0)
            ledcSetup(startChannel, LED_PWM_FREQUENCY, LED_PWM_RESOLUTION);
        if (l.gPin >= 0)
            ledcSetup(startChannel + 1, LED_PWM_FREQUENCY, LED_PWM_RESOLUTION);
        if (l.bPin >= 0)
            ledcSetup(startChannel + 2, LED_PWM_FREQUENCY, LED_PWM_RESOLUTION);

        if (l.rPin >= 0)
            ledcAttachPin(l.rPin, startChannel);
        if (l.gPin >= 0)
            ledcAttachPin(l.gPin, startChannel + 1);
        if (l.bPin >= 0)
            ledcAttachPin(l.bPin, startChannel + 2);
#endif
    }
#elif defined LED_USE_DMX

    dmx.init();
#else

    #ifdef LED_FET_PIN
    pinMode(LED_FET_PIN, OUTPUT);
    #endif

    setLedEnabled(true);


#ifdef LED_NUM_STRIPS
#define LED_START1 0
#if LED_NUM_STRIPS >= 1
#if defined LED_CLK_PIN1
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN1, LED_CLK_PIN1, LED_COLOR_ORDER>(leds + LED_START1, LED_COUNT1).setCorrection(TypicalLEDStrip);
#else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN1, LED_COLOR_ORDER>(leds + LED_START1, LED_COUNT1).setCorrection(TypicalLEDStrip);
#endif //CLK
#define LED_START2 LED_START1+LED_COUNT1

#if LED_NUM_STRIPS >= 2
#if defined LED_CLK_PIN2
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN2, LED_CLK_PIN2, LED_COLOR_ORDER>(leds + LED_START2, LED_COUNT2).setCorrection(TypicalLEDStrip);
#else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN2, LED_COLOR_ORDER>(leds + LED_START2, LED_COUNT2).setCorrection(TypicalLEDStrip);
#endif //CLK
#define LED_START3 LED_START2+LED_COUNT2

#if LED_NUM_STRIPS >= 3
#if defined LED_CLK_PIN3
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN3, LED_CLK_PIN3, LED_COLOR_ORDER>(leds + LED_START3, LED_COUNT3).setCorrection(TypicalLEDStrip);
#else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN3, LED_COLOR_ORDER>(leds + LED_START3, LED_COUNT3).setCorrection(TypicalLEDStrip);
#endif //CL
#define LED_START4 LED_START3+LED_COUNT3

#if LED_NUM_STRIPS >= 4
#if defined LED_CLK_PIN4
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN4, LED_CLK_PIN4, LED_COLOR_ORDER>(leds + LED_START4, LED_COUNT4).setCorrection(TypicalLEDStrip);
#else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN4, LED_COLOR_ORDER>(leds + LED_START4, LED_COUNT4).setCorrection(TypicalLEDStrip);
#endif //CLK
#define LED_START5 LED_START4+LED_COUNT4

#if LED_NUM_STRIPS >= 5
#if defined LED_CLK_PIN5
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN5, LED_CLK_PIN5, LED_COLOR_ORDER>(leds + LED_START5, LED_COUNT5).setCorrection(TypicalLEDStrip);
#else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN5, LED_COLOR_ORDER>(leds + LED_START5, LED_COUNT5).setCorrection(TypicalLEDStrip);
#endif //CLK

#endif //5
#endif //4
#endif //3
#endif //2
#endif //1
#else
#if defined LED_CLK_PIN
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLK_PIN, LED_COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
#else
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
#endif //CLK
#endif // MULTISTRIP
#endif //PWM / DMX / FastLED switch


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
    if(!ledEnabled) return;

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

    // NDBG("FastLed.show()");
    /*if(ledEnabled) */ 
    FastLED.show();

#endif
#endif //LED_COUNT
}

void RGBLedsManager::shutdown()
{
    setLedEnabled(false);
}

void RGBLedsManager::setBrightness(float value, bool save)
{
    globalBrightness = min(max(value, 0.f), 1.f);

#ifdef LED_COUNT
#ifdef LED_SEPARATE_CHANNELS
#elif defined LED_USE_DMX
#else
    FastLED.setBrightness((int)(globalBrightness * LED_MAX_BRIGHTNESS));
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

void RGBLedsManager::setLedEnabled(bool val)
{
    ledEnabled = val;
    FastLED.clear();
    FastLED.show();
#ifdef LED_EN_PIN
    NDBG("Set Led Enabled : "+String(val));
    pinMode(LED_EN_PIN, OUTPUT); //enable LEDs
    digitalWrite(LED_EN_PIN, val);
#endif

#ifdef LED_USE_FET
    NDBG("Set Led Enabled (FET) : "+String(val));
    digitalWrite(LED_FET_PIN, val);
    pinMode(LED_DATA_PIN, val ? OUTPUT : INPUT_PULLDOWN);
#endif
}

bool RGBLedsManager::handleCommand(String command, var *data, int numData)
{
#ifdef LED_COUNT
    if (checkCommand(command, "enabled", numData, 1))
    {
        setLedEnabled(data[0].intValue());
        return true;
    }
    else if (checkCommand(command, "brightness", numData, 1))
    {
        setBrightness(data[0].floatValue(), true);
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
    else if (checkCommand(command, "brightnessStatus", numData, 0))
    {
        float msgData[1] =  { globalBrightness };
        
        sendEvent(RGBLedsEvent(RGBLedsEvent::BrightnessStatus, msgData, 1));
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
    LedHelpers::point(leds, LED_COUNT, c, pos, fade, doClear);
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
