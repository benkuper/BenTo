#pragma once
class IOComponent :
    public Component
{
public:
    IOComponent(const String &name = "io", bool _enabled = true) : Component(name, _enabled) {}

    enum PinMode { D_INPUT,
                   D_INPUT_PULLUP,
                   A_INPUT,
                   D_OUTPUT,
                   A_OUTPUT };

Parameter * pin;
Parameter * mode;
Parameter * inverted;

Parameter *value;
float prevValue;

virtual bool initInternal(JsonObject o) override;
virtual void updateInternal() override;
virtual void clearInternal() override;

void setupPin();
void updatePin();

EndDeclareComponent