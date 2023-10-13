#include "../common/Common.h"

#ifdef HAS_SCRIPTS

#include "wasmFunctions.h"
#include "../MainManager.h"
#include <SimplexNoise.h>

SimplexNoise sn;

m3ApiRawFunction(m3_arduino_millis)
{
    m3ApiReturnType(uint32_t);

    m3ApiReturn(millis());
}

m3ApiRawFunction(m3_arduino_getTime)
{
    m3ApiReturnType(float);
    float v = millis() / 1000.0f - ScriptManager::instance->timeAtLaunch;
    m3ApiReturn(v);
}

m3ApiRawFunction(m3_arduino_delay)
{
    m3ApiGetArg(uint32_t, ms);
    delay(ms);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_printFloat)
{
    m3ApiGetArg(float, val);
    DBG("Print from script : " + String(val));
    m3ApiSuccess();
}

m3ApiRawFunction(m3_printInt)
{
    m3ApiGetArg(uint32_t, val);
    DBG("Print from script : " + String(val));
    m3ApiSuccess();
}

#ifdef LED_COUNT
m3ApiRawFunction(m3_clearLeds)
{
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.clear(LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_dimLeds)
{
    m3ApiGetArg(float, v);
    MainManager::instance->leds.rgbManager.dimLayer(v, LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLeds)
{
    m3ApiGetArg(uint32_t, color);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.fillAll(CRGB(color), false, LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLedsRGB)
{
    m3ApiGetArg(uint32_t, r);
    m3ApiGetArg(uint32_t, g);
    m3ApiGetArg(uint32_t, b);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.fillAll(CRGB((uint8_t)r, (uint8_t)g, (uint8_t)b), false, LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLedsHSV)
{
    m3ApiGetArg(uint32_t, h);
    m3ApiGetArg(uint32_t, s);
    m3ApiGetArg(uint32_t, v);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.fillAll(CHSV((uint8_t)h, (uint8_t)s, (uint8_t)v), false, LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_setLed)
{
    m3ApiGetArg(uint32_t, index);
    m3ApiGetArg(uint32_t, color);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CRGB(color), LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_getLed)
{
    m3ApiReturnType(uint32_t)
        m3ApiGetArg(uint32_t, index);

    if (index < LED_COUNT)
    {
        CRGB c = MainManager::instance->leds.rgbManager.leds[LED_BASE_LAYER][index];
        uint32_t val = c.r << 16 | c.g << 8 | c.b;
        m3ApiReturn(val);
    }

    m3ApiReturn(0)
}

m3ApiRawFunction(m3_setLedRGB)
{
    m3ApiGetArg(uint32_t, index);
    m3ApiGetArg(uint32_t, r);
    m3ApiGetArg(uint32_t, g);
    m3ApiGetArg(uint32_t, b);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CRGB((uint8_t)r, (uint8_t)g, (uint8_t)b), LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_setLedHSV)
{
    m3ApiGetArg(uint32_t, index);
    m3ApiGetArg(uint32_t, h);
    m3ApiGetArg(uint32_t, s);
    m3ApiGetArg(uint32_t, v);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CHSV((uint8_t)h, (uint8_t)s, (uint8_t)v), LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_pointRGB)
{
    m3ApiGetArg(float, pos);
    m3ApiGetArg(float, radius);
    m3ApiGetArg(uint32_t, h);
    m3ApiGetArg(uint32_t, s);
    m3ApiGetArg(uint32_t, v);

    MainManager::instance->leds.rgbManager.point(CRGB((uint8_t)h, (uint8_t)s, (uint8_t)v), pos, radius, false, LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_pointHSV)
{
    m3ApiGetArg(float, pos);
    m3ApiGetArg(float, radius);
    m3ApiGetArg(uint32_t, h);
    m3ApiGetArg(uint32_t, s);
    m3ApiGetArg(uint32_t, v);
    MainManager::instance->leds.rgbManager.point(CHSV((uint8_t)h, (uint8_t)s, (uint8_t)v), pos, radius, false, LED_SCRIPT_LAYER);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_setIR)
{
    m3ApiGetArg(float, v);
    MainManager::instance->leds.irManager.setBrightness(v);

    m3ApiSuccess();
}


m3ApiRawFunction(m3_updateLeds)
{
    MainManager::instance->leds.rgbManager.update();
    m3ApiSuccess();
}


m3ApiRawFunction(m3_getFXSpeed)
{
    m3ApiReturnType(float);
    float v = MainManager::instance->leds.fxManager.offsetSpeed;
    m3ApiReturn((float)v);
}

m3ApiRawFunction(m3_getFXIsoSpeed)
{
    m3ApiReturnType(float);
    float v = MainManager::instance->leds.fxManager.isolationSpeed;
    m3ApiReturn((float)v);
}

m3ApiRawFunction(m3_getFXStaticOffset)
{
    m3ApiReturnType(float);
    float v = MainManager::instance->leds.fxManager.staticOffset;
    m3ApiReturn((float)v);
}

m3ApiRawFunction(m3_getFXFlipped)
{
    m3ApiReturnType(uint32_t);
    bool v = MainManager::instance->leds.fxManager.boardIsFlipped;
    m3ApiReturn((uint32_t)v);
}

m3ApiRawFunction(m3_setFXSpeed)
{
    m3ApiGetArg(float, sp);
    MainManager::instance->leds.fxManager.offsetSpeed = sp;
    m3ApiSuccess();
}

m3ApiRawFunction(m3_setFXIsoSpeed)
{
    m3ApiGetArg(float, sp);
    MainManager::instance->leds.fxManager.isolationSpeed = sp;
    m3ApiSuccess();
}

m3ApiRawFunction(m3_setFXIsoAxis)
{
    m3ApiGetArg(uint32_t, ax);
    MainManager::instance->leds.fxManager.isolationAxis = ax;
    m3ApiSuccess();
}

m3ApiRawFunction(m3_setFXStaticOffset)
{
    m3ApiGetArg(float, sp);
    MainManager::instance->leds.fxManager.staticOffset = sp;
    m3ApiSuccess();
}

m3ApiRawFunction(m3_resetFX)
{
    MainManager::instance->leds.fxManager.reset();
    m3ApiSuccess();
}


#endif

m3ApiRawFunction(m3_setBatterySendEnabled)
{
    m3ApiGetArg(uint32_t, en);
    MainManager::instance->battery.setSendEnabled((bool)en);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_playVariant)
{
    m3ApiGetArg(uint32_t, v);
    String name = MainManager::instance->leds.playerMode.fileName;
    float time = MainManager::instance->leds.playerMode.curTimeMs;
    uint32_t start = millis();
    char l = name.charAt(name.length()-1);

    if (l >= '0' && l <= '9') {
        name.remove(name.length()-1);
    } 

    name = name + String(v);

    MainManager::instance->leds.playerMode.load(name, false);

    time = (time + (millis() - start)) / 1000;
    MainManager::instance->leds.playerMode.play(time);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_getOrientation)
{
    m3ApiReturnType(float);
    m3ApiGetArg(uint32_t, oi);

    float v = oi < 3 ? MainManager::instance->imu.orientation[oi] : -1;

    m3ApiReturn(v);
}

m3ApiRawFunction(m3_getYaw)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.orientation[0]);
}

m3ApiRawFunction(m3_getPitch)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.orientation[1]);
}
m3ApiRawFunction(m3_getRoll)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.orientation[2]);
}

m3ApiRawFunction(m3_getThrowState)
{
    m3ApiReturnType(uint32_t);
    m3ApiReturn((uint32_t)MainManager::instance->imu.throwState);
}

m3ApiRawFunction(m3_getProjectedAngle)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.projectedAngle);
}

m3ApiRawFunction(m3_setProjectedAngleOffset)
{
    m3ApiGetArg(float, yaw);
    m3ApiGetArg(float, angle);
    // MainManager::instance->imu.setProjectAngleOffset(yaw, angle);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_calibrateIMU)
{
    MainManager::instance->imu.calibrate();
    m3ApiSuccess();
}

m3ApiRawFunction(m3_setIMUEnabled)
{
    m3ApiGetArg(uint32_t, en);
    MainManager::instance->imu.setEnabled((bool)en);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_getButtonState)
{
    m3ApiReturnType(uint32_t);

    m3ApiGetArg(uint32_t, btIndex);

#ifdef BUTTON_COUNT
    int v = btIndex < BUTTON_COUNT ? MainManager::instance->buttons.isPressed[btIndex] : 0;
#else
    int v = 0;
#endif

    m3ApiReturn((uint32_t)v);
}

m3ApiRawFunction(m3_getActivity)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.activity);
}

m3ApiRawFunction(m3_getSpin)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.spin);
}

m3ApiRawFunction(m3_setMicEnabled)
{
    m3ApiGetArg(uint32_t, en);
    MicManager::instance->setEnabled((bool)en);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_getMicLevel)
{
    m3ApiReturnType(float);

#ifdef BUTTON_COUNT
    float v = MicManager::instance->enveloppe;
#else
    float v = 0;
#endif

    m3ApiReturn((float)v);
}


m3ApiRawFunction(m3_randomInt)
{
    m3ApiReturnType(uint32_t);
    m3ApiGetArg(uint32_t, min);
    m3ApiGetArg(uint32_t, max);

    m3ApiReturn((uint32_t)random(min, max + 1));
}

m3ApiRawFunction(m3_noise)
{
    m3ApiReturnType(float);
    m3ApiGetArg(float, x);
    m3ApiGetArg(float, y);

    float n = (float)sn.noise(x, y);
    n = (n + 1) / 2; // convert to value range [0..1]

    m3ApiReturn(n);
}

#endif