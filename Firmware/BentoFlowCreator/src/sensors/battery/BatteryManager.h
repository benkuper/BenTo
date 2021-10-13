#pragma once
#include "../../common/Common.h"

#define PREV_VALUES_SIZE 10

class BatteryEvent
{
public:
    enum Type { Level, Voltage, RawValue, CriticalLevel, Charging, Reset, TYPES_MAX};

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

#ifndef BATTERY_DEFAULT_MIN
#define BATTERY_DEFAULT_MIN 222 //TESTED: 3.2V
#endif

#ifndef BATTERY_DEFAULT_MAX
#define BATTERY_DEFAULT_MAX 335 // TESTED: 3.8V
#endif

    const int defaultMinVal = BATTERY_DEFAULT_MIN; 
    const int defaultMaxVal = BATTERY_DEFAULT_MAX; 

    int minVal = defaultMinVal;
    int maxVal = defaultMaxVal;

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
    void setMax(int value, bool save = false);
    void resetMax();


private:
#ifdef USE_PREFERENCES
    Preferences prefs;
#elif defined USE_SETTINGS_MANAGER
    SettingsManager prefs;
#endif
};