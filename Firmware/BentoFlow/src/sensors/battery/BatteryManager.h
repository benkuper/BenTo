#pragma once
#include "../../common/Common.h"

#define PREV_VALUES_SIZE 10

class BatteryEvent
{
public:
    enum Type { Level, Voltage, RawValue, CriticalLevel, Charging, Reset, SendEnabled, TYPES_MAX};

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

    int prevRawValues[PREV_VALUES_SIZE];
    int curPos = 0;

    bool isCharging;
    bool sendEnabled;

#ifndef BATTERY_RAW_MIN
#define BATTERY_RAW_MIN 205 //TESTED: 3.5V
#endif

#ifndef BATTERY_RAW_MAX
#define BATTERY_RAW_MAX 324 // TESTED: 4.2V
#endif

#ifndef BATTERY_VOLTAGE_MIN
#define BATTERY_VOLTAGE_MIN 3.5
#endif

#ifndef BATTERY_VOLTAGE_MAX
#define BATTERY_VOLTAGE_MAX 4.2
#endif


    int minVal = BATTERY_RAW_MIN;
    int maxVal = BATTERY_RAW_MAX;

    bool isCriticalBattery;

    const float criticalBatteryThreshold = 0.0f;
    const long criticalBatteryTimethreshold = 5000; // 5s
    long timeAtCriticalBattery;

    const long batteryReadTime = 1000;
    long timeSinceLastBatteryRead;

    void init();
    void update();

    bool handleCommand(String command, var *data, int numData) override;

    void updateValue(int currentValue);
    void updateMax(int currentValue);
    void updateCharge();
    void setMin(int value, bool save = false);
    void setMax(int value, bool save = false);
    void resetMax();
    void setSendEnabled(bool value);



private:
#ifdef USE_PREFERENCES
    Preferences prefs;
#elif defined USE_SETTINGS_MANAGER
    SettingsManager prefs;
#endif
};