#include "LedHelpers.h"
#include "../../common/DebugHelpers.h"

void LedHelpers::clear(CRGB *leds, int numLeds)
{
    memset(leds, 0, numLeds * sizeof(CRGB));
}

void LedHelpers::fillAll(CRGB *leds, int numLeds, CRGB c)
{
    LedHelpers::fillRange(leds, numLeds, c, 0, 1);
}

void LedHelpers::fillRange(CRGB *leds, int numLeds, CRGB c, float start, float end, bool doClear)
{
    if (doClear)
        clear(leds, numLeds);

    int s = max(min(start, end), 0.f) * (numLeds - 1);
    int e = min(max(start, end), 1.f) * (numLeds - 1);

    for (int i = s; i <= e; i++)
    {
        leds[i] += c;
    }
}

void LedHelpers::point(CRGB *leds, int numLeds, CRGB c, float pos, float radius, bool doClear)
{

    if (doClear)
        clear(leds, numLeds);

    if(radius == 0) return;
    
    for (int i = 0; i < numLeds; i++)
    {
        float rel = i * 1.0f / (numLeds - 1);
        float fac = max(1 - (std::abs((float)(pos - rel)) / radius), 0.f);
        leds[i] += CRGB(c.r * fac, c.g * fac, c.b * fac);
    }
}
