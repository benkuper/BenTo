#pragma once

#define BATTERY_CHECK_INTERVAL 100
#define BATTERY_SET_INTERVAL 1000
#define VALUES_SIZE 10

DeclareComponentSingleton(Battery, "battery", )

    DeclareIntParam(batteryPin, BATTERY_DEFAULT_PIN);
DeclareIntParam(chargePin, -1);
DeclareIntParam(rawMin, BATTERY_DEFAUT_RAW_MIN);
DeclareIntParam(rawMax, BATTERY_DEFAULT_RAW_MAX);

DeclareBoolParam(sendFeedback, true);

DeclareFloatParam(batteryLevel, 4.2f);
DeclareBoolParam(charging, false);

long lastBatteryCheck = 0;
long lastBatterySet = 0;
float values[VALUES_SIZE];
int valuesIndex = 0;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

HandleSetParamInternalStart
    CheckAndSetParam(batteryPin);
CheckAndSetParam(chargePin);
CheckAndSetParam(rawMin);
CheckAndSetParam(rawMax);
CheckAndSetParam(sendFeedback);
CheckAndSetParam(batteryLevel);
CheckAndSetParam(charging);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(batteryPin);
FillSettingsParam(chargePin);
FillSettingsParam(rawMin);
FillSettingsParam(rawMax);
FillSettingsParam(sendFeedback);
FillSettingsParam(batteryLevel);
FillSettingsParam(charging);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryIntParam(batteryPin);
FillOSCQueryIntParam(chargePin);
FillOSCQueryIntParam(rawMin);
FillOSCQueryIntParam(rawMax);
FillOSCQueryBoolParam(sendFeedback);
FillOSCQueryFloatParamReadOnly(batteryLevel);
FillOSCQueryBoolParamReadOnly(charging);
FillOSCQueryInternalEnd

    EndDeclareComponent