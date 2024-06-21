ImplementManagerSingleton(IO);

bool IOComponent::initInternal(JsonObject o)
{
    pwmChannel = -1;

    AddIntParamConfig(pin);
    AddIntParamConfig(mode);

    // mode.options = modeOptions;
    // mode.numOptions = PINMODE_MAX;

    AddBoolParamConfig(inverted);

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
        RootComponent::availablePWMChannels[pwmChannel] = true;
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
        case D_OSC:
            pinm = OUTPUT;
            break;

        default:
            break;
        }

        if (pinm != -1)
        {

            pinMode(curPin, pinm);
        }
        else
        {
            if (m == A_OUTPUT || m == A_OSC)
            {
                int channel = RootComponent::instance->getFirstAvailablePWMChannel();
                if (channel >= 0)
                {
                    pwmChannel = channel;
                    ledcSetup(pwmChannel, 5000, 10); // 0-1024 at a 5khz resolution
                    ledcAttachPin(curPin, pwmChannel);
                    RootComponent::availablePWMChannels[pwmChannel] = false;
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

        if (value != val)
        {
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

    case D_OSC:
    {
        bool v = (millis() % (int)(value * 1000)) > (value * 300);
        if(value == 0) v = 0;
        else if(value == 1) v = 1;
        
        if (inverted)
            v = !v;
        digitalWrite(pin, v);
    }
    break;

    case A_OSC:
    {
        if (pwmChannel != -1)
        {
            float sv = sin(millis() * value) * 0.5f + 0.5f;
            uint32_t v = sv * 1024;
            // NDBG("Set PWM with value " + String(v));
            ledcWrite(pwmChannel, v);
        }
    }
    break;
    }
}