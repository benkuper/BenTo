bool ServoComponent::initInternal(JsonObject o)
{
    AddIntParamConfig(pin);
    if (pin > 0)
        servo.attach(pin);

    AddFloatParam(position);
    return true;
}

void ServoComponent::updateInternal()
{
}

void ServoComponent::clearInternal()
{
    servo.detach();
}

void ServoComponent::paramValueChangedInternal(void *param)
{
    NDBG("param changed");
    if (param == &pin)
    {
        servo.detach();
        if (pin > 0)
        {
            NDBG("Attach to pin " + String(pin));
            servo.attach(pin);
        }
    }

    if (param == &position)
    {
        NDBG("Set position " + String(position));
        servo.write(position);
    }
}