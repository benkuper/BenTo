#pragma once
DeclareComponentSingleton(Battery, "battery", )

DeclareConfigParameter(rawMin, BATTERY_DEFAUT_RAW_MIN);
DeclareConfigParameter(rawMax, BATTERY_DEFAULT_RAW_MAX);
DeclareConfigParameter(lowVoltage, BATTERY_DEFAULT_LOW_VOLTAGE);
DeclareConfigParameter(sendFeedback, true);

DeclareRangeParameter(batteryLevel, 4.2f, 3.f, 4.2f, false);
DeclareParameter(isCharging, false, false);


bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent