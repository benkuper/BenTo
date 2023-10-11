#pragma once

DeclareComponent(Servo, "servo", )

DeclareIntParam(pin, 0);
DeclareFloatParam(position, 0);

Servo servo;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

// void onParameterEventInternal(const ParameterEvent &e) override;

   HandleSetParamInternalStart
        CheckAndSetParam(pin);
    CheckAndSetParam(position);
    HandleSetParamInternalEnd;

    FillSettingsInternalStart
        FillSettingsParam(pin);
    FillSettingsParam(position);
    FillSettingsInternalEnd

        FillOSCQueryInternalStart
            FillOSCQueryIntParam(pin);
    FillOSCQueryFloatParam(position);
    FillOSCQueryInternalEnd

EndDeclareComponent