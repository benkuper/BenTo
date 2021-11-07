#include "wasmFunctions.h"
#include "../common/Common.h"
#include "../MainManager.h"

m3ApiRawFunction(m3_arduino_millis)
{
    m3ApiReturnType (uint32_t)

    m3ApiReturn(millis());
}

m3ApiRawFunction(m3_arduino_delay)
{
    m3ApiGetArg     (uint32_t, ms)

    // You can also trace API calls
    //Serial.print("api: delay "); Serial.println(ms);

    delay(ms);

    m3ApiSuccess();
}

// This maps pin modes from arduino_wasm_api.h
// to actual platform-specific values
uint8_t mapPinMode(uint8_t mode)
{
    switch(mode) {
    case 0: return INPUT;
    case 1: return OUTPUT;
    case 2: return INPUT_PULLUP;
    }
    return INPUT;
}

m3ApiRawFunction(m3_arduino_pinMode)
{
    m3ApiGetArg     (uint32_t, pin)
    m3ApiGetArg     (uint32_t, mode)

#if !defined(PARTICLE)
    typedef uint8_t PinMode;
#endif
    pinMode(pin, (PinMode)mapPinMode(mode));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_digitalWrite)
{
    m3ApiGetArg     (uint32_t, pin)
    m3ApiGetArg     (uint32_t, value)

    digitalWrite(pin, value);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_getPinLED)
{
    m3ApiReturnType (uint32_t)

    m3ApiReturn(13);
}

m3ApiRawFunction(m3_arduino_print)
{
    m3ApiGetArgMem  (const uint8_t *, buf)
    m3ApiGetArg     (uint32_t,        len)

    //printf("api: print %p %d\n", buf, len);
    Serial.write(buf, len);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_getGreeting)
{
    m3ApiGetArgMem  (uint8_t *,    out)
    m3ApiGetArg     (uint32_t,     out_len)

    const char buff[] = "Hello WASM world! 😊";
    memcpy(out, buff, min(sizeof(buff), out_len));

    m3ApiSuccess();
}


m3ApiRawFunction(m3_clearLeds)
{
    MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.clear();
 
    m3ApiSuccess();
}


m3ApiRawFunction(m3_fillLeds)
{
    m3ApiGetArg     (uint32_t, color)
    MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.fillAll(CRGB(color));
 
    m3ApiSuccess();
}

m3ApiRawFunction(m3_setLed)
{
    m3ApiGetArg     (uint32_t, index)
    m3ApiGetArg     (uint32_t, color)
    MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CRGB(color));
 
    m3ApiSuccess();
}

m3ApiRawFunction(m3_setLedRGB)
{
    m3ApiGetArg     (uint32_t, index)
    m3ApiGetArg     (uint32_t, r)
    m3ApiGetArg     (uint32_t, g)
    m3ApiGetArg     (uint32_t, b)
    MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CRGB((uint8_t)r,(uint8_t)g,(uint8_t)b));
 
    m3ApiSuccess();
}

m3ApiRawFunction(m3_setLedHSV)
{
    m3ApiGetArg     (uint32_t, index)
    m3ApiGetArg     (uint32_t, h)
    m3ApiGetArg     (uint32_t, s)
    m3ApiGetArg     (uint32_t, v)
    MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CHSV((uint8_t)h,(uint8_t)s,(uint8_t)v));
 
    m3ApiSuccess();
}