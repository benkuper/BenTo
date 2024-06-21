bool DCMotorComponent::initInternal(JsonObject o)
{
    AddIntParamConfig(enPin);
    AddIntParamConfig(dir1Pin);
    AddIntParamConfig(dir2Pin);

    AddFloatParam(speed);

    if (dir1Pin >= 0 && dir2Pin >= 0 && enPin >= 0)
    {
        pinMode(dir1Pin, OUTPUT);
        pinMode(dir2Pin, OUTPUT);

        int channel = RootComponent::instance->getFirstAvailablePWMChannel();
        if (channel >= 0)
        {
            pwmChannel = channel;
            ledcSetup(pwmChannel, 5000, 10); // 0-1024 at a 5khz resolution
            ledcAttachPin(enPin, pwmChannel);
            RootComponent::availablePWMChannels[pwmChannel] = false;
            // NDBG("Attach pin " + String(pin) + " to " + String(pwmChannel));
        }
        else
        {
            NDBG("Max channels reached, not able to create speedPin PWM channel");
        }
    }
}

void DCMotorComponent::updateInternal()
{
}

void DCMotorComponent::clearInternal()
{
}

void DCMotorComponent::paramValueChangedInternal(void *param)
{
    if (param == &speed)
    {
        digitalWrite(dir1Pin, speed > 0 ? HIGH : LOW);
        digitalWrite(dir2Pin, speed > 0 ? LOW : HIGH);
        NDBG("Speed changed " + String(speed)+" / "+String(dir1Pin)+" :" + (speed > 0 ? "HIGH" : "LOW") + " / "+String(dir2Pin)+" :" + (speed > 0 ? "LOW" : "HIGH"));
        ledcWrite(pwmChannel, abs(speed) * 1024.0f);
    }
}