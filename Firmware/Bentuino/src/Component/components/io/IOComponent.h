#pragma once
DeclareComponent(IO, "io", )

    enum PinMode { D_INPUT,
                   D_INPUT_PULLUP,
                   A_INPUT,
                   D_OUTPUT,
                   A_OUTPUT };

DeclareConfigParameter(pin, 0);
DeclareConfigParameter(mode, D_INPUT);
DeclareConfigParameter(inverted, false);

int pwmChannel;
static int pwmChannelCount;

Parameter value{"value", 0.f, 0.f, 1.f};
float prevValue;

virtual bool initInternal(JsonObject o) override;
virtual void updateInternal() override;
virtual void clearInternal() override;

virtual void setupPin();
void updatePin();

// LinkScriptFunctionsStart
//     LinkScriptFunction(IOComponent, get, f, );
// LinkScriptFunction(IOComponent, set, , f);
// LinkScriptFunctionsEnd

// DeclareScriptFunctionReturn0(IOComponent, get, float)
// {
//     return value.floatValue();
// }
// DeclareScriptFunctionVoid1(IOComponent, set, float) { return value.set(arg1); }

EndDeclareComponent