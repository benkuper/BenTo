bool IOComponent::initInternal()
{
    pin = GetIntConfig("pin");
    mode = GetIntConfig("mode");
    value = addParameter("value", 0.0f);
    prevValue = value->val;

    setupPin();
    updatePin();

    return true;
}

void IOComponent::updateInternal()
{
    updatePin();
}

void IOComponent::clearInternal()
{
}

void IOComponent::setupPin()
{
    if (pin != -1)
    {
        pinMode(pin, mode);
    }
}

void IOComponent::updatePin()
{
    if (pin == -1)
        return;

    switch (mode)
    {
    case D_INPUT:
    case D_INPUT_PULLUP:
        value->set(digitalRead(pin));
        break;

    case D_OUTPUT:
    case A_OUTPUT:
    {
        if (prevValue != (float)value->val)
        {
            if (mode == D_OUTPUT)
                digitalWrite(pin, (bool)value->val);
            else
            {
                // analogWrite
            }

            prevValue = (float)value->val;
        }
    }
    break;

    case A_INPUT:
        value->set(analogRead(pin) / 4095.0f);
        break;
    }
}