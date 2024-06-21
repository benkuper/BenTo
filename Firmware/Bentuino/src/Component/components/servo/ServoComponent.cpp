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
    if (param == &pin)
    {
        servo.detach();
        if (pin > 0)
        {
            servo.attach(pin);
        }
    }

    if (param == &position)
    {
        servo.write(position);
    }
}