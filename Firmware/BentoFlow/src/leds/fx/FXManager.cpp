
#include "FXManager.h"
#include "../../MainManager.h"

FXManager::FXManager(CRGB *leds, CRGB *outLeds) : Component("fx"),
                                                  leds(leds),
                                                  outLeds(outLeds),
                                                  curTime(0),
                                                  offsetSpeed(0),
                                                  isolationSpeed(0),
                                                  isolationSmoothing(0),
                                                  prevIsolationAngle(0),
                                                  isolationAxis(0)
{
}

void FXManager::update()
{
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

        bool boardIsUp = fabsf(MainManager::instance->imu.orientation[2]) > 90;   // roll is upsidown (hoop test)
        bool yawIsNormal = fabsf(MainManager::instance->imu.orientation[0]) < 90; // yaw is whitin calibration pose range

        if (boardIsUp != yawIsNormal)
            isoAngle = 1 - isoAngle;

        float targetIsoAngle = fmodf(isoAngle * isolationSpeed, 1);
        if (targetIsoAngle < 0)
            targetIsoAngle++;

        float diff = abs(targetIsoAngle - prevIsolationAngle);
        if (diff < .5)
            targetIsoAngle = prevIsolationAngle + (targetIsoAngle - prevIsolationAngle) * (1 - isolationSmoothing);

        prevIsolationAngle = targetIsoAngle;
        angleOffset = targetIsoAngle;

        // NDBG("Update " + String(isoAngle)); // + ", " +String(isolationSmoothing)+", diff "+String(diff));
    }

    angleOffset = fmodf(angleOffset + curTime, 1);
    if (angleOffset < 0)
        angleOffset++;

    if (angleOffset != 0)
    {
        memset(outLeds, 0, LED_COUNT * sizeof(CRGB));
        for (int i = 0; i < LED_COUNT; i++)
        {
            int index = int(i + angleOffset * LED_COUNT) % LED_COUNT;
            outLeds[i] = leds[index];
        }
    }
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

    return false;
}