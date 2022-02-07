bool IOComponent::initInternal()
{
    pin = GetIntConfig("pin");
    mode = GetIntConfig("mode");
    inverted = GetBoolConfig("inverted");

    if (mode == D_INPUT || mode == D_INPUT_PULLUP || mode == D_OUTPUT)
        value = addParameter("value", false);
    else
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
    {
        bool val = digitalRead(pin);
        if (inverted)
            val = !val;
        value->set(val);
    }
    break;

    case D_OUTPUT:
    case A_OUTPUT:
    {
        if (prevValue != value->floatValue())
        {
            if (mode == D_OUTPUT)
                digitalWrite(pin, inverted ? !value->boolValue() : value->boolValue());
            else
            {
                // analogWrite
            }

            prevValue = value->floatValue();
        }
    }
    break;

    case A_INPUT:
    {
        float v = analogRead(pin) / 4095.0f;
        if (inverted)
            v = 1 - v;
        value->set(v);
    }
    break;
    }
}