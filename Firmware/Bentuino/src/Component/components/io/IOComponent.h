#pragma once
DeclareComponent(IO, "io", )

    enum PinMode { D_INPUT,
                   D_INPUT_PULLUP,
                   A_INPUT,
                   D_OUTPUT,
                   A_OUTPUT };

Parameter pin{"pin", 0, var(), var(), true};
Parameter mode{"mode", D_INPUT, var(), var(), true};
Parameter inverted{"inverted", false, var(), var(), true};
int pwmChannel;
static int pwmChannelCount;

Parameter value{"value", 0.f, 0.f, 1.f};
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
    return value.floatValue();
}
DeclareScriptFunctionVoid1(IOComponent, set, float) { return value.set(arg1); }

EndDeclareComponent