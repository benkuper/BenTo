bool ServoComponent::initInternal(JsonObject o)
{
    AddAndSetParameter(pin);
    if (pin.intValue() > 0)
        servo.attach(pin.intValue());

    AddParameter(position);
    return true;
}

void ServoComponent::updateInternal()
{
}

void ServoComponent::clearInternal()
{
    servo.detach();
}

void ServoComponent::onParameterEventInternal(const ParameterEvent &e)
{
    if (e.parameter == &pin)
    {
        servo.detach();
        if (pin.intValue() > 0)
            servo.attach(pin.intValue());
    }
    if (e.parameter == &position)
    {
        servo.write(position.intValue());
    }
}