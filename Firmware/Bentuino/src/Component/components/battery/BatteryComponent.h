#pragma once

#define BATTERY_CHECK_INTERVAL 100
#define BATTERY_SET_INTERVAL 1000
#define VALUES_SIZE 10

DeclareComponentSingleton(Battery, "battery", )

DeclareConfigParameter(batteryPin, BATTERY_DEFAULT_PIN);
DeclareConfigParameter(chargePin, -1);
DeclareConfigParameter(rawMin, BATTERY_DEFAUT_RAW_MIN);
DeclareConfigParameter(rawMax, BATTERY_DEFAULT_RAW_MAX);

DeclareConfigParameter(sendFeedback, true);

DeclareRangeParameter(batteryLevel, 4.2f, 3.5f, 4.2f, false);
DeclareParameter(isCharging, false, false);


long lastBatteryCheck = 0;
long lastBatterySet = 0;
float values[VALUES_SIZE];
int valuesIndex = 0;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent