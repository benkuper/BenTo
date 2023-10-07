
#pragma once;

#include "common/Common.h"

#if HAS_LED_FX

#ifdef LED_COUNT
#include <FastLED.h>

class FXManager : public Component
{
public:
   FXManager(CRGB *leds, CRGB *outLeds);
   ~FXManager() {}

   CRGB *leds;
   CRGB *outLeds;

   float curTime;
   float prevTime;

   float staticOffset;
   float offsetSpeed;

   float isolationSpeed;
   float isolationSmoothing;
   float prevIsolationAngle;

   int isolationAxis; // 0 = projectedAngle, 1 = yaw, 2 = pitch, 3 = roll
   
   bool swapOnFlip;
   bool boardIsFlipped;
   int flipFrameCount;
   int flipDebounce;

   bool showCalibration;

   void update();
   void reset();

   bool handleCommand(String command, var *data, int numData) override;
};
#endif

#endif