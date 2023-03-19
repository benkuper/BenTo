
#pragma once;

#include "common/Common.h"
#include <FastLED.h>

class FXManager : public Component {
public:
   FXManager(CRGB* leds, CRGB* outLeds) ;
   ~FXManager(){}

   CRGB* leds;
   CRGB* outLeds;

   float curTime;
   float prevTime;

   float offsetSpeed;

   float isolationSpeed;
   float isolationSmoothing;
   float prevIsolationAngle;
   int isolationAxis; //0 = projectedAngle, 1 = yaw, 2 = pitch, 3 = roll

   void update();
   
   bool handleCommand(String command, var *data, int numData) override;

};
    
