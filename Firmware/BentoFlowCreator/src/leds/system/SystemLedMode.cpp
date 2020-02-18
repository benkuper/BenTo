
#include "SystemLedMode.h"

SystemLedMode::SystemLedMode(CRGB *leds, int numLeds) : LedMode("systemLedMode", leds, numLeds),
                                                        connectionState(Off),
                                                        timeAtStateChange(millis()),
                                                        pointPos(0),
                                                        uploadProgress(0)
{
}

void SystemLedMode::init()
{
}

void SystemLedMode::update()
{
   if (uploadFeedback)
   {
      LedHelpers::fillRange(leds, numLeds, CRGB::Purple, 1 - uploadProgress, 1);
      LedHelpers::point(leds, numLeds, CRGB::White, 1 - uploadProgress, .1f, false);

      return;
   }

   float relT = millis() / 1000.0f - timeAtStateChange;
   const float animTime = 2.0f; //2 second anim time

   if (connectionState == Off || connectionState != Connecting && relT > animTime)
      return;

   CRGB color = CRGB::Cyan;

   //default behavior (connecting) on which we will add animation for connected behavior
   float t = millis() / 1000.0f - timeAtConnecting;
   float pos = cos((t + PI) * 5) * .5f + .5f;
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
   uploadProgress = value;
}
