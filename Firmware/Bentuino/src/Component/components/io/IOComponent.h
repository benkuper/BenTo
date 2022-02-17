#pragma once
class IOComponent : public Component
{
public:
    IOComponent(ComponentType t = Type_IO) : Component(t) {}
    virtual ~IOComponent() {}

    enum PinMode
    {
        D_INPUT,
        D_INPUT_PULLUP,
        A_INPUT,
        D_OUTPUT,
        A_OUTPUT
    };

    Parameter *pin;
    Parameter *mode;
    Parameter *inverted;
    int pwmChannel;
    static int pwmChannelCount;

    Parameter *value;
    float prevValue;

    virtual bool initInternal(JsonObject o) override;
    virtual void updateInternal() override;
    virtual void clearInternal() override;

    virtual void setupPin();
    void updatePin();

    LinkScriptFunctionsStart
        LinkScriptFunction(IOComponent, get, f, );
    LinkScriptFunction(IOComponent, set, , f);
    LinkScriptFunctionsEnd

    DeclareScriptFunctionReturn0(IOComponent, get, float)
    {
        return value->floatValue();
    }
    DeclareScriptFunctionVoid1(IOComponent, set, float) { return value->set(arg1); }
};