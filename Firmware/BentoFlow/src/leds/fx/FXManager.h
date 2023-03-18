
#pragma once;

#include "common/Common.h"
#include <FastLED.h>

class FXManager : public Component {
public:
   FXManager(CRGB* leds, CRGB* outLeds) ;
   ~FXManager(){}

   CRGB* leds;
   CRGB* outLeds;

   void update();
};
    
