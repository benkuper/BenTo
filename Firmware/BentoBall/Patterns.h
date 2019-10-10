#ifndef PATTERNS_H
#define PATTERNS_H

#include "BatteryManager.h"
#include "FastLEDDefs.h"

#define MAX_PARAMS 8
class Pattern
{
  public:
    float params[MAX_PARAMS];
    long millisAtStart;
    long relMillis;
    Pattern() {
      Serial.println("Pattern constructor");
      millisAtStart = millis();
      relMillis = 0;
      memset(params, 0, MAX_PARAMS * sizeof(float));
    }

    void updatePattern() {
      relMillis = millis() - millisAtStart;
      update();
    }
    virtual void update() {}
#if USE_OSC
    virtual boolean handleMessage(OSCMessage &msg, int offset)
    {
      if (msg.match("/param", offset))
      {
        float value = msg.isFloat(1) ? msg.getFloat(1) : (msg.isInt(1) ? (float)msg.getInt(1) : 1);
        int index = min(max(msg.getInt(0), 0), MAX_PARAMS);
        params[index] = value;

        DBG("Set pattern param : "+String(index)+" = "+String(value));
      }
    }
#endif //USE_OSC
};

class SnapshotPattern : public Pattern
{
  public:
    SnapshotPattern() {
      showSnapshot();
    }
    virtual void update() override { }
};

class BatteryLevelPattern : public Pattern
{
  public:
    BatteryLevelPattern() { }
    virtual void update() override
    {
#if USE_BATTERY
      BatteryManager * bm = BatteryManager::instance;
      if (bm != nullptr)
      {
        int h = bm->normalizedVoltage * 96; //max voltage is light green, low voltage is red
        int v = bm->normalizedVoltage * NUM_LEDS;
        setRange(NUM_LEDS - v, NUM_LEDS, CHSV(h, 255, 100), true);
      }
#endif //USE_BATTERY
    }
};


class LedRepairPattern : public Pattern
{
  public:
    LedRepairPattern() { }
    virtual void update() override
    {
      setFullColor(CHSV(relMillis * 10, 255, 50));
    }
};

class SolidColorPattern : public Pattern
{
  public:
    CRGB color;
    float height = 1;
    float smoothing = .4f;
    SolidColorPattern(CRGB _color) : color(_color)
    {
      params[0] = height;
    }

    virtual void update() override
    {
      height = height + ((float)params[0] - height) * smoothing;
      int v = height * NUM_LEDS;
      setRange(NUM_LEDS - v, NUM_LEDS, color, true);
    }
};

class FishPattern : public Pattern
{
  public:
    int index;

    FishPattern(int increment = 1, int frameDelay = 2, float hueSpeed = .1f) {
      index = 0;
      params[0] = increment;
      params[1] = frameDelay;
      params[2] = hueSpeed;
    }

    virtual void update() override
    {
      int increment = (int)params[0];
      int frameDelay = (int)params[1];
      float hueSpeed = params[2];

      int h = relMillis * hueSpeed;
      for (int i = 0; i < NUM_LEDS; i++)
      {
        if (i == index) simpleSetLed(i, CHSV(h, 255, 200));
        else simpleSetLed(i, 0);
      }
      index = (index + increment) % NUM_LEDS;
      delay(frameDelay);
    }


};

class CheckerPattern : public Pattern
{
  public:
    int index;

    CheckerPattern(int modulo = 2, int switchTime = 0, int frameDelay = 2000, int hueShift = 60, float hueSpeed = .05f) {
      index = 0;
      params[0] = modulo;
      params[1] = frameDelay;
      params[2] = switchTime;
      params[3] = hueShift;
      params[4] = hueSpeed;
    }

    virtual void update() override
    {
      int modulo = max((int)params[0], 1);
      int frameDelay = (int)params[1];
      int switchTime = (int)params[2];
      int hueShift = (int)params[3];
      float hueSpeed = params[4];

      float h = relMillis * hueSpeed;
      bool doSwitch = switchTime <= 0 || (relMillis % switchTime * 2) > switchTime;

      if (!doSwitch) h += 60;
      int mi = 0;
      for (int i = 0; i < NUM_LEDS; i++)
      {

        if ((i + index) % modulo == 0)
        {
          simpleSetLed(i, CHSV(h + mi * hueShift, 255, 200));
          mi++;
        }
        else simpleSetLed(i, 0);
      }
      if (doSwitch) index = (index + 1) % modulo;
      delayMicroseconds(frameDelay);

    }
};

#endif
