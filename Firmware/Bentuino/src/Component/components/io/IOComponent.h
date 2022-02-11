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

virtual void setupPin();
void updatePin();

LinkScriptFunctionsStart
LinkScriptFunction(IOComponent, get,f,);
LinkScriptFunction(IOComponent, set,,f);
LinkScriptFunctionsEnd

DeclareScriptFunctionReturn0(IOComponent, get,float) { return value->floatValue(); }
DeclareScriptFunctionVoid1(IOComponent, set,float)  { return value->set(arg1); }

EndDeclareComponent