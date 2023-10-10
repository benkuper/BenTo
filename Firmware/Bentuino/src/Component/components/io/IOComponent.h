#pragma once
DeclareComponent(IO, "io", )

    enum PinMode { D_INPUT,
                   D_INPUT_PULLUP,
                   A_INPUT,
                   D_OUTPUT,
                   A_OUTPUT,
                   PINMODE_MAX };

DeclareConfigParameter(pin, -1);
DeclareConfigParameter(mode, D_INPUT);
DeclareConfigParameter(inverted, false);

int pwmChannel;
int curPin;

Parameter value{"value", 0.f, 0.f, 1.f};
float prevValue;

const String modeOptions[PINMODE_MAX]{"Digital Input", "Digital Input Pullup", "Analog Input", "Digital Output", "Analog Output"};

virtual bool initInternal(JsonObject o) override;
virtual void updateInternal() override;
virtual void clearInternal() override;

virtual void setupPin();
void updatePin();

void onParameterEventInternal(const ParameterEvent &e) override;

static bool availablePWMChannels[16];
int getFirstAvailablePWMChannel() const;

#ifdef USE_SCRIPT
LinkScriptFunctionsStart
    LinkScriptFunction(IOComponent, get, f, );
LinkScriptFunction(IOComponent, set, , f);
LinkScriptFunctionsEnd

DeclareScriptFunctionReturn0(IOComponent, get, float)
{
    return value.floatValue();
}
DeclareScriptFunctionVoid1(IOComponent, set, float) { return value.set(arg1); }
#endif

EndDeclareComponent;


//Manager

DeclareComponentSingleton(IOManager, "GPIO", )

    DeclareRangeConfigParameter(numIOs, IO_MAX_COUNT, 0, IO_MAX_COUNT);

IOComponent ios[IO_MAX_COUNT];

bool initInternal(JsonObject o) override;

EndDeclareComponent