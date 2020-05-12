#pragma once
#include "../../common/Common.h"


class BatteryEvent
{
public:
    enum Type { Level, Voltage, RawValue, CriticalLevel, Charging, TYPES_MAX};

    static const String eventNames[TYPES_MAX];

    BatteryEvent(Type type, float value) : type(type), value(value) {}
    Type type;
    float value;
};

class BatteryManager : public Component, public EventBroadcaster<BatteryEvent> {
public:
    BatteryManager();
    ~BatteryManager(){}

    int rawValue;
    float value;
    float voltage;

    bool isCharging;

    const int minVal = 864; //TESTED
    const int maxVal = 1460; //TO FIND

    bool isCriticalBattery;

    const float criticalBatteryThreshold = 0.3f;
    const long criticalBatteryTimethreshold = 5000; // 5s
    long timeAtCriticalBattery;

    const long batterySendTime = 1000;
    long timeSinceLastBatterySent;

    void init();
    void update();
};