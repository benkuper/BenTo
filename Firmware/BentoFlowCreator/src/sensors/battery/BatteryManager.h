#pragma once
#include "../../common/Common.h"


class BatteryEvent
{
public:
    enum Type { Level, Voltage, CriticalLevel, Charging, Discharging, TYPES_MAX};

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

    const int minVal = 1200;
    const int maxVal = 1880;

    const long batterySendTime = 1000;
    long timeSinceLastBatterySent;

    void init();
    void update();
};