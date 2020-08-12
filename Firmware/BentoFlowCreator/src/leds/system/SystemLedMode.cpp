
#include "SystemLedMode.h"

#ifdef LED_COUNT

SystemLedMode::SystemLedMode(CRGB *leds, int numLeds) : LedMode("systemLedMode", leds, numLeds),
                                                        connectionState(Off),
                                                        timeAtStateChange(millis()),
                                                        pointPos(0),
                                                        uploadProgress(0),
                                                        uploadFeedback(false)
{
}

void SystemLedMode::init()
{
}

bool SystemLedMode::update()
{
   LedHelpers::clear(leds, numLeds);

#ifndef NO_ANIMATIONS

   float relT = millis() / 1000.0f - timeAtStateChange;
   const float animTime = 2.0f; //2 second anim time

   if (connectionState == Off || connectionState != Connecting && relT > animTime)
      return true;

   CRGB color = CRGB::Cyan;

   //default behavior (connecting) on which we will add animation for connected behavior
   float t = millis() / 1000.0f - timeAtConnecting;
   float pos = cos((t + PI) * 5) * .5f + .5f;

#ifdef LED_INVERT_DIRECTION
   pos = 1-pos;
#endif

   float radius = .3 - (cos(pos * PI * 2) * .5f + .5f) * .25f;

   color.nscale8(min(t, 1.f) * 255);

   if (connectionState != Connecting)
   {
      CRGB targetColor;
      switch (connectionState)
      {
      case Connected:
         targetColor = CRGB::Green;
         break;
      case ConnectionError:
         targetColor = CRGB::Red;
         break;
      case Disabled:
         targetColor = CRGB::Purple;
         break;
      case Hotspot:
         targetColor = CRGB::Yellow;
         break;
      }

      float blendFac = min(relT * 2, 1.f);
      radius = max(radius, relT * 2); //increase radius to 1 in one second
      float fade = 1 - min(max((relT - 1) / .5f, 0.f), 1.f);
      targetColor.nscale8(fade * 255);

      color = blend(color, targetColor, blendFac * 255);
   }

   LedHelpers::point(leds, numLeds, color, pos, radius, true);

#endif

   return true;
}

void SystemLedMode::setConnectionState(ConnectionState state)
{
   connectionState = state;
   timeAtStateChange = millis() / 1000.0f;
   if (connectionState == Connecting)
      timeAtConnecting = timeAtStateChange;
}

void SystemLedMode::showUploadProgress(float value)
{

#ifndef NO_ANIMATIONS
   uploadProgress = fmodf(value*2,1);
   int curPage = floor(value*2);
   uploadFeedback = true;
   
   if(curPage > 0) LedHelpers::fillRange(leds, numLeds, CHSV((curPage-1)*40,255,255), 0, uploadProgress);
   else LedHelpers::clear(leds, numLeds);
   LedHelpers::fillRange(leds, numLeds, CHSV(curPage*40,255,255), uploadProgress, 1, false);
   LedHelpers::point(leds, numLeds, CRGB::White, uploadProgress, .1f, false);

#if !defined LEDS_SEPARATE_CHANNELS && !defined LED_USE_DMX
   FastLED.show(); //force here because no update in leds when uploading apparently
#endif

#endif

}

#endif