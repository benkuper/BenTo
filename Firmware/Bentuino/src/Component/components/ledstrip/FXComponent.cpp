
#ifndef FX_SWAP_UPSIDE_DOWN
#define FX_SWAP_UPSIDE_DOWN 0
#endif

bool FXComponent::initInternal(JsonObject o)
{
    AddFloatParam(staticOffset);
    AddFloatParam(offsetSpeed);
    AddFloatParam(isolationSpeed);
    AddFloatParam(isolationSmoothing);
    AddIntParam(isolationAxis);
    AddBoolParam(swapOnFlip);
    AddBoolParam(showCalibration);

    memset(colors, 0, strip->count * sizeof(Color));

    return true;
}

void FXComponent::updateInternal()
{
}

void FXComponent::clearInternal()
{
}

void FXComponent::process(Color *sourceColors)
{

    int numLeds = strip->count;
    if (numLeds == 0)
        return;

    if (showCalibration)
    {
        memset(colors, 0, numLeds * sizeof(Color));
        colors[0] = Color(255, 0, 0);
        colors[numLeds / 6] = Color(0, 255, 0);
        return;
    }

    float t = millis() / 1000.0;
    float deltaTime = t - prevTime;
    prevTime = t;

    curTime += deltaTime * offsetSpeed;

    float angleOffset = 0;

#if USE_MOTION
    MotionComponent *motion = &RootComponent::instance->motion;
    if (motion->enabled && isolationSpeed != 0 && isolationAxis >= 0 && isolationAxis < 4)
    {
        float isoAngle = 0;
        switch (isolationAxis)
        {
        case ProjectedAngle:
            isoAngle = motion->projectedAngle;
            break;

        case Yaw:
            isoAngle = (motion->orientation[0] + 180) / 360;

            break;

        case Pitch:
            isoAngle = (motion->orientation[1] + 90) / 180;
            break;

        case Roll:
            isoAngle = (motion->orientation[2] + 180) / 360;
            break;
        }

        if (swapOnFlip)
        {
            bool boardIsUp = fabsf(motion->orientation[2]) > 90;   // roll is upsidown (hoop test)
            bool yawIsNormal = fabsf(motion->orientation[0]) < 90; // yaw is whitin calibration pose range

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
#endif

    angleOffset = staticOffset + fmodf(angleOffset + curTime, 1);
    if (angleOffset < 0)
        angleOffset++;

    if (angleOffset != 0 || boardIsFlipped)
    {

        for (int i = 0; i < numLeds; i++)
        {
            int ledIndex = boardIsFlipped ? numLeds - 1 - i : i;
            int index = int(ledIndex + angleOffset * numLeds) % numLeds;

            colors[i] = sourceColors[index];

            memcpy(sourceColors, colors, numLeds * sizeof(Color)); //copy back colors into ledstrip's colors array
        }
    }

}

void FXComponent::reset()
{
    NDBG("Reset FX");
    curTime = 0;
    SetParam(staticOffset, 0);
    SetParam(offsetSpeed, 0);
    SetParam(isolationSpeed, 0);
}