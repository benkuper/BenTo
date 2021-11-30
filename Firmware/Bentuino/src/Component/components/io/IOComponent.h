#pragma once
DeclareComponent(IO, "io",)

    enum PinMode { D_INPUT, D_INPUT_PULLUP, A_INPUT, D_OUTPUT, A_OUTPUT };

    Parameter<int> * pin;
    Parameter<int> * mode;
    Parameter<float> * value;
    float prevValue;

    void initInternal() override
    {
        pin = addParameter<int>("pin", -1);
        mode = addParameter<int>("mode", D_OUTPUT);
        value = addParameter<float>("value", 0);
        prevValue = value->val;

        setupPin();
        updatePin();
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
        if(pin->val != -1)
        {
            pinMode(pin->val, mode->val);
        }
    }

    void updatePin()
    {
        if(pin->val == -1) return;

        switch(mode->val)
        {
            case D_INPUT:
            case D_INPUT_PULLUP:
                value->set(digitalRead(pin->val));
                break;

            case D_OUTPUT:
            case A_OUTPUT:
            {
                if(prevValue != value->val)
                {
                    if(mode->val == D_OUTPUT) digitalWrite(pin->val, (bool)value->val);
                    else
                    {
                        //analogWrite
                    }

                    prevValue = value->val;
                }
            }
            break;

            case A_INPUT:
                value->set(analogRead(pin->val) / 4095.0f);
            break;
        }
    }

EndDeclareComponent