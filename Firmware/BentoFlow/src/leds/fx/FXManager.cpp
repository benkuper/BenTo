
#include "FXManager.h"
#include "../../MainManager.h"

#ifdef HAS_LED_FX

#ifdef LED_COUNT

#ifndef FX_SWAP_UPSIDE_DOWN
#define FX_SWAP_UPSIDE_DOWN 0
#endif

FXManager::FXManager(CRGB *leds, CRGB *outLeds) : Component("fx"),
                                                  leds(leds),
                                                  outLeds(outLeds),
                                                  curTime(0),
                                                  offsetSpeed(0),
                                                  isolationSpeed(0),
                                                  isolationSmoothing(.5f),
                                                  prevIsolationAngle(0),
                                                  isolationAxis(0),
                                                  swapOnFlip(FX_SWAP_UPSIDE_DOWN),
                                                  boardIsFlipped(false),
                                                  flipFrameCount(0),
                                                  flipDebounce(50),
                                                  showCalibration(false)
{
}

void FXManager::update()
{
    if (showCalibration)
    {
        LedHelpers::clear(outLeds, LED_COUNT);
        outLeds[0] = CRGB::Red;
        outLeds[LED_COUNT / 6] = CRGB::Green;
        return;
    }

    float t = millis() / 1000.0;
    float deltaTime = t - prevTime;
    prevTime = t;

    curTime += deltaTime * offsetSpeed;

    float angleOffset = 0;

    if (isolationSpeed != 0 && isolationAxis >= 0 && isolationAxis < 4)
    {
        float isoAngle = 0;
        switch (isolationAxis)
        {
        case 0:
            isoAngle = MainManager::instance->imu.projectedAngle;
            break;

        case 1:
            isoAngle = (MainManager::instance->imu.orientation[0] + 180) / 360;

            break;

        case 2:
            isoAngle = (MainManager::instance->imu.orientation[1] + 90) / 180;
            break;

        case 3:
            isoAngle = (MainManager::instance->imu.orientation[2] + 180) / 360;
            break;
        }

        if (swapOnFlip)
        {
            bool boardIsUp = fabsf(MainManager::instance->imu.orientation[2]) > 90;   // roll is upsidown (hoop test)
            bool yawIsNormal = fabsf(MainManager::instance->imu.orientation[0]) < 90; // yaw is whitin calibration pose range

            bool flipped = boardIsUp != yawIsNormal;
            if (flipped != boardIsFlipped)
            {
                DBG("Flip count > " + String(flipFrameCount) + " / " + String(flipFrameCount));
                flipFrameCount++;
                if (flipFrameCount >= flipDebounce)
                {
                    DBG("FLIP NOW !");
                    boardIsFlipped = flipped;
                    flipFrameCount = 0;
                }
            }

            if (boardIsFlipped)
            {
                isoAngle += .5; // offset half, and the led loop will also reverse the source colors grab
            }
        }

        float targetIsoAngle = fmodf(isoAngle * isolationSpeed, 1);
        if (targetIsoAngle < 0)
            targetIsoAngle++;

        float diff = abs(targetIsoAngle - prevIsolationAngle);
        if (diff < .5)
            targetIsoAngle = prevIsolationAngle + (targetIsoAngle - prevIsolationAngle) * (1 - isolationSmoothing);

        prevIsolationAngle = targetIsoAngle;
        angleOffset = targetIsoAngle;
    }

    angleOffset = staticOffset + fmodf(angleOffset + curTime, 1);
    if (angleOffset < 0)
        angleOffset++;

    if (angleOffset != 0 || boardIsFlipped)
    {
        memset(outLeds, 0, LED_COUNT * sizeof(CRGB));

        for (int i = 0; i < LED_COUNT; i++)
        {
            int ledIndex = boardIsFlipped ? LED_COUNT - 1 - i : i;
            int index = int(ledIndex + angleOffset * LED_COUNT) % LED_COUNT;

            outLeds[i] = leds[index];
        }
    }
}

void FXManager::reset()
{
    NDBG("Reset FX");
    curTime = 0;
    staticOffset = 0;
    offsetSpeed = 0;
    isolationSpeed = 0;
}

bool FXManager::handleCommand(String command, var *data, int numData)
{
    NDBG("Handle command : " + command);
    if (checkCommand(command, "isolationAxis", numData, 1))
    {
        isolationAxis = data[0].intValue();
        return true;
    }
    else if (checkCommand(command, "isolationSpeed", numData, 1))
    {
        isolationSpeed = data[0].floatValue();
        return true;
    }
    else if (checkCommand(command, "isolationSmoothing", numData, 1))
    {
        isolationSmoothing = data[0].floatValue();
        return true;
    }
    else if (checkCommand(command, "offsetSpeed", numData, 1))
    {
        offsetSpeed = data[0].floatValue();
        return true;
    }
    else if (checkCommand(command, "staticOffset", numData, 1))
    {
        staticOffset = data[0].floatValue();
        return true;
    }
    else if (checkCommand(command, "flipDebounce", numData, 1))
    {
        flipDebounce = data[0].floatValue();
        return true;
    }
    else if (checkCommand(command, "reset", numData, 0))
    {
        reset();
        return true;
    }
    else if (checkCommand(command, "showCalibration", numData, 1))
    {
        showCalibration = (bool)data[0].intValue();
        return true;
    }

    return false;
}
#endif

#endif