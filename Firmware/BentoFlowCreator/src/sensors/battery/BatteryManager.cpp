#include "BatteryManager.h"

const String BatteryEvent::eventNames[BatteryEvent::TYPES_MAX]{"level", "voltage", "rawValue", "criticalLevel", "charging", "discharging"};

BatteryManager::BatteryManager() : Component("battery"),
                                   voltage(4.1f),
                                   rawValue(0),
                                   value(1),
                                   timeSinceLastBatterySent(0),
                                   isCriticalBattery(false),
                                   timeAtCriticalBattery(0)
{
}

void BatteryManager::init()
{

#ifdef BATTERY_PIN
    analogSetPinAttenuation(BATTERY_PIN, ADC_0db);
#endif
}

void BatteryManager::update()
{
#ifdef BATTERY_PIN
    long curTime = millis();
    rawValue = analogRead(BATTERY_PIN);
    value = (rawValue - minVal)*1.0f / (maxVal-minVal);
    voltage = (rawValue * 10 / 4.0f) / 1000;
        
    if(curTime > timeSinceLastBatterySent + batterySendTime)
    {
        sendEvent(BatteryEvent(BatteryEvent::Level, value));
        sendEvent(BatteryEvent(BatteryEvent::Voltage, voltage));
        sendEvent(BatteryEvent(BatteryEvent::RawValue, rawValue));
        timeSinceLastBatterySent = curTime;
    }

    bool batteryIsOK = value >= criticalBatteryThreshold;
    if(batteryIsOK) 
    {
        timeAtCriticalBattery = 0;
        isCriticalBattery = false;
    }
    else{
        if(timeAtCriticalBattery == 0) timeAtCriticalBattery = curTime;
        else if(curTime > timeAtCriticalBattery + criticalBatteryTimethreshold)
        {
            isCriticalBattery = true;
            sendEvent(BatteryEvent(BatteryEvent::CriticalLevel, value));
        }
    }
#endif
}
