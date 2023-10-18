#pragma once

#define BATTERY_CHECK_INTERVAL 100
#define BATTERY_SET_INTERVAL 1000
#define VALUES_SIZE 10

DeclareComponentSingleton(Battery, "battery", )

    DeclareIntParam(batteryPin, BATTERY_DEFAULT_PIN);
DeclareIntParam(chargePin, BATTERY_DEFAULT_CHARGE_PIN);
DeclareIntParam(rawMin, BATTERY_DEFAUT_RAW_MIN);
DeclareIntParam(rawMax, BATTERY_DEFAULT_RAW_MAX);
DeclareFloatParam(lowBatteryThreshold, 3.5);

DeclareBoolParam(sendFeedback, true);

DeclareFloatParam(batteryLevel, 4.2f);
DeclareBoolParam(charging, false);

long lastBatteryCheck = 0;
long lastBatterySet = 0;
long timeAtLowBattery = 0;
float values[VALUES_SIZE];
int valuesIndex = 0;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

Color getBatteryColor();

DeclareComponentEventTypes(CriticalBattery);
DeclareComponentEventNames("CriticalBattery");

CheckFeedbackParamInternalStart
    CheckAndSendParamFeedback(batteryLevel);
CheckFeedbackParamInternalEnd;

HandleSetParamInternalStart
    CheckAndSetParam(batteryPin);
CheckAndSetParam(chargePin);
CheckAndSetParam(rawMin);
CheckAndSetParam(rawMax);
CheckAndSetParam(lowBatteryThreshold);
CheckAndSetParam(sendFeedback);
CheckAndSetParam(batteryLevel);
CheckAndSetParam(charging);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(batteryPin);
FillSettingsParam(chargePin);
FillSettingsParam(rawMin);
FillSettingsParam(rawMax);
FillSettingsParam(lowBatteryThreshold);
FillSettingsParam(sendFeedback);
FillSettingsParam(batteryLevel);
FillSettingsParam(charging);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryIntParam(batteryPin);
FillOSCQueryIntParam(chargePin);
FillOSCQueryIntParam(rawMin);
FillOSCQueryIntParam(rawMax);
FillOSCQueryRangeParam(lowBatteryThreshold, 3.3, 4.2)
    FillOSCQueryBoolParam(sendFeedback);
FillOSCQueryRangeParamReadOnly(batteryLevel, lowBatteryThreshold, 4.2f);
FillOSCQueryBoolParamReadOnly(charging);
FillOSCQueryInternalEnd

    EndDeclareComponent