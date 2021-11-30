#pragma once
DeclareComponent(IO, "io",)

    enum PinMode { D_INPUT, D_INPUT_PULLUP, A_INPUT, D_OUTPUT, A_OUTPUT };

    int pin;
    int mode;
    Parameter<float> * value;
    float prevValue;

    bool initInternal() override
    {
        pin = GetIntConfig("pin");
        mode = GetIntConfig("mode");
        value = addParameter<float>("value", 0);
        prevValue = value->val;

        setupPin();
        updatePin();

        return true;
    }

    void updateInternal()
    {
        updatePin();
    }

    void clearInternal()
    {

    }

    void setupPin()
    {
        if(pin != -1)
        {
            pinMode(pin, mode);
        }
    }

    void updatePin()
    {
        if(pin == -1) return;

        switch(mode)
        {
            case D_INPUT:
            case D_INPUT_PULLUP:
                value->set(digitalRead(pin));
                break;

            case D_OUTPUT:
            case A_OUTPUT:
            {
                if(prevValue != value->val)
                {
                    if(mode == D_OUTPUT) digitalWrite(pin, (bool)value->val);
                    else
                    {
                        //analogWrite
                    }

                    prevValue = value->val;
                }
            }
            break;

            case A_INPUT:
                value->set(analogRead(pin) / 4095.0f);
            break;
        }
    }

EndDeclareComponent