bool ServoComponent::initInternal(JsonObject o)
{
    AddIntParam(pin);
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

// void ServoComponent::onParameterEventInternal(const ParameterEvent &e)
// {
//     if (e.parameter == &pin)
//     {
//         servo.detach();
//         if (pin > 0)
//             servo.attach(pin);
//     }
//     if (e.parameter == &position)
//     {
//         servo.write(position);
//     }
// }