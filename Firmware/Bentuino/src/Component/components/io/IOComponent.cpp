int IOComponent::pwmChannelCount = 0;

bool IOComponent::initInternal(JsonObject o)
{
    pwmChannel = -1;

    pin = AddConfigParameter("pin", 0);
    mode = AddConfigParameter("mode", D_INPUT);
    inverted = AddConfigParameter("inverted", false);

    int m = mode->intValue();

    if (m == D_INPUT || m == D_INPUT_PULLUP || m == D_OUTPUT)
        value = AddParameter("value", false);
    else
        value = AddRangeParameter("value", 0.0f, 0.0f, 1.0f, false);

    value->readOnly = m == D_INPUT || m == D_INPUT_PULLUP || m == D_OUTPUT;

    prevValue = value->floatValue();

    setupPin();
    updatePin();

    return true;
}

void IOComponent::updateInternal()
{
    if (enabled->boolValue())
        updatePin();
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
            pinm = OUTPUT;
            break;

        default:
            break;
        }

        if (pinm != -1)
        {
            pinMode(pin->intValue(), pinm);
        }
        else if (m == A_OUTPUT)
        {
            if (m == A_OUTPUT)
            {
                if (pwmChannelCount < 15)
                {
                    pwmChannel = pwmChannelCount;
                    ledcSetup(pwmChannel, 5000, 10); // 0-1024 at a 5khz resolution
                    ledcAttachPin(pin->intValue(), pwmChannel);
                    pwmChannelCount++;
                    NDBG("Attach pin "+pin->stringValue()+" to "+String(pwmChannel));
                }
                else
                {
                    NDBG("Max channels reached, not able to create another A_OUTPUT");
                }
            }
        }
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
            {
                digitalWrite(pin->intValue(), inverted ? !value->boolValue() : value->boolValue());
            }
            else
            {
                if (pwmChannel != -1)
                {
                    uint32_t v = value->floatValue() * 1024;
                    NDBG("Set PWM with value "+String(v));
                    ledcWrite(pwmChannel, v);
                }
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