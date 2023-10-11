ImplementManagerSingleton(IO);

bool IOComponent::availablePWMChannels[16] = {true};

bool IOComponent::initInternal(JsonObject o)
{
    pwmChannel = -1;

    AddIntParam(pin);
    AddIntParam(mode);

    // mode.options = modeOptions;
    // mode.numOptions = PINMODE_MAX;

    AddBoolParam(inverted);

    AddFloatParam(value);
    // int m = mode;
    // value.readOnly = m == D_INPUT || m == D_INPUT_PULLUP || m == A_INPUT;

    prevValue = value;

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
    if (curPin != -1 && pwmChannel != -1) // prevPin was a PWM pin
    {
        ledcDetachPin(curPin);
        availablePWMChannels[pwmChannel] = true;
        pwmChannel = -1;
    }

    curPin = pin;

    if (curPin > 0)
    {
        int m = mode;

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

            pinMode(curPin, pinm);
        }
        else if (m == A_OUTPUT)
        {
            if (m == A_OUTPUT)
            {
                int channel = getFirstAvailablePWMChannel();
                if (channel >= 0)
                {
                    pwmChannel = channel;
                    ledcSetup(pwmChannel, 5000, 10); // 0-1024 at a 5khz resolution
                    ledcAttachPin(curPin, pwmChannel);
                    availablePWMChannels[pwmChannel] = false;
                    // NDBG("Attach pin " + String(pin) + " to " + String(pwmChannel));
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
    if (pin == -1)
        return;

    int m = mode;
    switch (m)
    {
    case D_INPUT:
    case D_INPUT_PULLUP:
    {
        bool val = digitalRead(pin);
        if (inverted)
            val = !val;

        if(value != val) {
            SetParam(value, val);
        }
        // SetParam(value, val);
    }
    break;

    case D_OUTPUT:
    case A_OUTPUT:
    {
        if (prevValue != value)
        {
            if (m == D_OUTPUT)
            {
                digitalWrite(pin, inverted ? !value : value);
            }
            else
            {
                if (pwmChannel != -1)
                {
                    uint32_t v = value * 1024;
                    // NDBG("Set PWM with value " + String(v));
                    ledcWrite(pwmChannel, v);
                }
            }

            prevValue = value;
        }
    }
    break;

    case A_INPUT:
    {
        float v = analogRead(pin) / 4095.0f;
        if (inverted)
            v = 1 - v;
        SetParam(value, v);
    }
    break;
    }
}

// void IOComponent::onParameterEventInternal(const ParameterEvent &e)
// {
//     if (e.parameter == &mode)
//     {
//         int m = mode;
//         // value.readOnly = m == D_INPUT || m == D_INPUT_PULLUP || m == A_INPUT;
//         setupPin();
//     }
//     else if (e.parameter == &pin)
//     {
//         setupPin();
//     }
// }

int IOComponent::getFirstAvailablePWMChannel() const
{
    for (int i = 0; i < 16; i++)
    {
        if (availablePWMChannels[i])
            return i;
    }
    return -1;
}
