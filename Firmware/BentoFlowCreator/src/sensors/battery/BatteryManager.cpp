#include "BatteryManager.h"

const String BatteryEvent::eventNames[BatteryEvent::TYPES_MAX]{"level", "voltage", "rawValue", "criticalLevel", "charging", "reset"};

BatteryManager::BatteryManager() : Component("battery"),
                                   voltage(4.1f),
                                   rawValue(0),
                                   value(1),
                                   isCharging(false),
                                   timeSinceLastBatteryRead(0),
                                   isCriticalBattery(false),
                                   timeAtCriticalBattery(0)
{
}

void BatteryManager::init()
{

#ifdef BATTERY_CHARGE_PIN
    pinMode(BATTERY_CHARGE_PIN, INPUT);
#endif

#ifdef BATTERY_PIN
    analogSetPinAttenuation(BATTERY_PIN, ADC_0db);

#ifdef USE_PREFERENCES
    prefs.begin(name.c_str());
    setMax(prefs.getInt("max", defaultMaxVal), false);
    prefs.end();
#elif defined USE_SETTINGS_MANAGER
    //init once with a json if it doesn't exist yet
    prefs.readSettings(String("/" + name + ".json").c_str());
    float max = prefs.getInt("max", defaultMaxVal);
    prefs.loadJson(String("{\"max\":\"" + String(max) + "\"}").c_str());
    prefs.writeSettings(String("/" + name + ".json").c_str());

    //actually read the data
    prefs.readSettings(String("/" + name + ".json").c_str());
    setMax(prefs.getInt("max", defaultMaxVal), false);
#endif

#endif // BATTERY_PIN
}

void BatteryManager::update()
{
#ifdef BATTERY_PIN

   long curTime = millis();

   if (curTime > timeSinceLastBatteryRead + batteryReadTime)
   {
         timeSinceLastBatteryRead = curTime;


#ifdef CUSTOM_BATTERY_READ
        rawValue = getBatteryRawValue();
        voltage = getBatteryVoltage(rawValue);
        value = (voltage - BATTERY_VOLTAGE_MIN) * (BATTERY_VOLTAGE_MAX - BATTERY_VOLTAGE_MIN);
        isCharging = isChargingBattery();
#else
        rawValue = analogRead(BATTERY_PIN);
        voltage = (rawValue - defaultMinVal) * (3.8 - 3.2) / (defaultMaxVal - defaultMinVal) + 3.2;
        updateValue(rawValue);
        updateMax(rawValue);
        updateCharge();
#endif

        sendEvent(BatteryEvent(BatteryEvent::Level, value));
        sendEvent(BatteryEvent(BatteryEvent::Voltage, voltage));
        sendEvent(BatteryEvent(BatteryEvent::RawValue, rawValue));
#ifdef BATTERY_CHARGE_PIN
        sendEvent(BatteryEvent(BatteryEvent::Charging, isCharging));
#endif

        bool batteryIsOK = value > criticalBatteryThreshold;
        if (batteryIsOK)
        {
            timeAtCriticalBattery = 0;
            isCriticalBattery = false;
        }
        else
        {
            if (timeAtCriticalBattery == 0)
                timeAtCriticalBattery = curTime;
            else if (curTime > timeAtCriticalBattery + criticalBatteryTimethreshold)
            {
                isCriticalBattery = true;
                sendEvent(BatteryEvent(BatteryEvent::CriticalLevel, value));
            }
        }


    }

#endif
}

void BatteryManager::updateMax(int currentValue)
{
    if (currentValue - 10 > maxVal)
    {
        setMax(currentValue - 10, true);
    }
}

void BatteryManager::setMax(int newMax, bool save)
{
    maxVal = max(newMax, 0);

    if (save)
    {
#ifdef USE_PREFERENCES
        prefs.begin(name.c_str());
        prefs.putInt("max", maxVal);
        prefs.end();
#elif defined USE_SETTINGS_MANAGER
        prefs.readSettings(String("/" + name + ".json").c_str());
        prefs.setInt("max", maxVal);
#endif
    }
}

void BatteryManager::resetMax()
{
    maxVal = defaultMaxVal;
    setMax(defaultMaxVal, true);

    sendEvent(BatteryEvent(BatteryEvent::Reset, maxVal));
}

void BatteryManager::updateValue(int newValue)
{
    long sum = 0L;
    prevRawValues[curPos] = newValue;

    for (int i = 0; i < PREV_VALUES_SIZE; i++)
    {
        sum += prevRawValues[i];
    }

    int avgRawValue = (int)(((float)sum) / PREV_VALUES_SIZE);
    value = (avgRawValue - minVal) * 1.0f / (maxVal - minVal);

    curPos = (curPos + 1) % PREV_VALUES_SIZE;
}

void BatteryManager::updateCharge()
{
#ifdef BATTERY_CHARGE_PIN
    int measuredVal = 0;
    for (int i = 0; i < 10; i++)
    {
        int v = digitalRead(BATTERY_CHARGE_PIN);
        measuredVal += v;
    }

    isCharging = measuredVal == 0;
#endif
}

bool BatteryManager::handleCommand(String command, var *data, int numData)
{
#ifdef BATTERY_PIN
    if (checkCommand(command, "reset", numData, 0))
    {
        resetMax();
        return true;
    }
#endif

    return false;
}