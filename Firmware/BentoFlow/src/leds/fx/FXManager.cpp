
#include "FXManager.h"
#include "../../MainManager.h"

FXManager::FXManager(CRGB *leds, CRGB *outLeds)
    : Component("fx"), leds(leds), outLeds(outLeds)
{
}

void FXManager::update()
{
    // float t = millis() / 1000.0f; //

    memset(outLeds, 0, LED_COUNT * sizeof(CRGB));

    float angle = MainManager::instance->imu.projectedAngle;

    for (int i = 0; i < LED_COUNT; i++)
    {
        int index = int(i + angle * LED_COUNT) % LED_COUNT;
        outLeds[i] = leds[index];
    }
}