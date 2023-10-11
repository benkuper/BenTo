#pragma once
DeclareComponent(Stepper, "stepper", )

    static FastAccelStepperEngine engine;
static bool engineIsInit;
FastAccelStepper *stepper = NULL;

DeclareIntParam(stepPin, 0);
DeclareIntParam(dirPin, 0);
DeclareIntParam(enPin, 0);

DeclareFloatParam(accel, 0);
DeclareFloatParam(speed, 0);
DeclareFloatParam(position, 0);

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

// void onParameterEventInternal(const ParameterEvent &e) override;

HandleSetParamInternalStart
    CheckAndSetParam(stepPin);
CheckAndSetParam(dirPin);
CheckAndSetParam(enPin);
CheckAndSetParam(accel);
CheckAndSetParam(speed);
CheckAndSetParam(position);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(stepPin);
FillSettingsParam(dirPin);
FillSettingsParam(enPin);
FillSettingsParam(accel);
FillSettingsParam(speed);
FillSettingsParam(position);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryIntParam(stepPin);
FillOSCQueryIntParam(dirPin);
FillOSCQueryIntParam(enPin);
FillOSCQueryFloatParam(accel);
FillOSCQueryFloatParam(speed);
FillOSCQueryFloatParam(position);
FillOSCQueryInternalEnd

    EndDeclareComponent