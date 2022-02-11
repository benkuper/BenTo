bool ServoComponent::initInternal(JsonObject o)
{
    pin = AddConfigParameter("pin", 0);
    if (pin > 0)
        servo.attach(pin->intValue());

    position = AddParameter("position", 0, 0, 180);
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
    if (e.parameter == pin)
    {
        servo.detach();
        if (pin > 0)
            servo.attach(pin->intValue());
    }
    if (e.parameter == position)
    {
        servo.write(position->intValue());
    }
}