#pragma once

#ifndef SERVO_DEFAULT_PIN
#define SERVO_DEFAULT_PIN 13
#endif

DeclareComponent(Servo, "servo", )

DeclareIntParam(pin, SERVO_DEFAULT_PIN);
DeclareFloatParam(position, 0);

Servo servo;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;
void paramValueChangedInternal(void *param) override;

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