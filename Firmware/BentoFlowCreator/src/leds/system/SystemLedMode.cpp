
#include "SystemLedMode.h"

SystemLedMode::SystemLedMode(CRGB * leds, int numLeds) :
   LedMode("systemLedMode", leds, numLeds),
   connectionState(Off),
   timeAtStateChange(millis()),
   pointPos(0)
{
   
}

void SystemLedMode::init()
{

}

void SystemLedMode::update()
{   
   switch(connectionState)
   {
      case Off: break;

      case Connecting:
      {
         const long t = millis();
         float pos = cos(t / 100.0f) * .5f + .5f;
         LedHelpers::point(leds, numLeds, CRGB::Cyan, pos, .5f,true);
      } 
      break;

      case Connected:
      {
         LedHelpers::fillAll(leds, numLeds, CRGB::Green);
      }
      break;

      case ConnectionError:
      {
         LedHelpers::fillAll(leds, numLeds, CRGB::Red);
      }
      break;

      case Disabled:
      {
         LedHelpers::fillAll(leds, numLeds, CRGB::Yellow);
      }

      case Hotspot:
      {
         LedHelpers::fillAll(leds, numLeds, CRGB::Yellow);
      }
      break;
   }
}

bool SystemLedMode::handleCommand(String command, var * data, int numData) 
{
   return false; 
}

void SystemLedMode::setConnectionState(ConnectionState state) {
   NDBG("Set connection state "+connectionStateNames[state]);
   connectionState = state;
   timeAtStateChange = millis();
}
