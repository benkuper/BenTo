bool IOComponent::initInternal(JsonObject o)
{
    pin = addConfigParameter("pin", 0);
    mode = addConfigParameter("mode", D_INPUT);
    inverted = addConfigParameter("inverted", false);

    int m = mode->intValue();

    if (m == D_INPUT || m == D_INPUT_PULLUP || m == D_OUTPUT)
        value = addParameter("value", false);
    else
        value = addParameter("value", 0.0f);

    value->readOnly = true;

    prevValue = value->floatValue();

    setupPin();
    updatePin();

    return true;
}

void IOComponent::updateInternal()
{
    if(enabled->boolValue()) updatePin();
}

void IOComponent::clearInternal()
{
}

void IOComponent::setupPin()
{
    if (pin->intValue() != -1)
    {
        int m = mode->intValue();

        int pinm = -1;
        switch (m)
        {
        case D_INPUT:
        case A_INPUT:
            pinm = INPUT;
            break;

        case D_INPUT_PULLUP:
            pinm = INPUT_PULLUP;
            break;

        case D_OUTPUT:
        case A_OUTPUT:
            pinm = OUTPUT;
            break;
        }

        if (pinm != -1)
            pinMode(pin->intValue(), pinm);
    }
}

void IOComponent::updatePin()
{
    if (pin->intValue() == -1)
        return;

    int m = mode->intValue();
    switch (m)
    {
    case D_INPUT:
    case D_INPUT_PULLUP:
    {
        bool val = digitalRead(pin->intValue());
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
            if (m == D_OUTPUT)
                digitalWrite(pin->intValue(), inverted ? !value->boolValue() : value->boolValue());
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
        float v = analogRead(pin->intValue()) / 4095.0f;
        if (inverted)
            v = 1 - v;
        value->set(v);
    }
    break;
    }
}