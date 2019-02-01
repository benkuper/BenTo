#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#define BATT_CHECK_INTERVAL 500

#define BATTERY_PIN 32

#define VOLTAGE_MIN 2300
#define VOLTAGE_MAX 2800
#define VOLTAGE_CHARGING 4090

#define BATT_MIN 3.1
#define BATT_MAX 4.2
#define BATT_LOW 3.5
#define BATT_HIGH 3.9

#define CRITICAL_SHUT_TIME 10000 //shut off after 10s critical

class BatteryManager
{
  public:
    static BatteryManager * instance;

    int rawData;
    float voltage;
    float normalizedVoltage;

    bool isCharging;
    long lastCheckTime;

    long criticalLevelEnterTime;

    BatteryManager()
    {
      instance = this;


      addBatteryCriticalLevelCallback(&BatteryManager::criticalDefaultCallback);
      addChargingStateChangedCallback(&BatteryManager::chargingStateChangedDefaultCallback);
      addBatteryLevelUpdateCallback(&BatteryManager::batteryLevelUpdateDefaultCallback);

      lastCheckTime = millis();
      isCharging = false;
    }

    void init()
    {
      pinMode(BATTERY_PIN, INPUT);
    }

    void update()
    {
      checkBattery();
    }

    float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    void checkBattery(bool force = false)
    {

      if (force || millis() - lastCheckTime > BATT_CHECK_INTERVAL)
      {
        lastCheckTime = millis();

        rawData = analogRead(BATTERY_PIN);
        setCharging(rawData >= VOLTAGE_CHARGING);

        float newVoltage = mapFloat(rawData, VOLTAGE_MIN, VOLTAGE_MAX, BATT_MIN, BATT_MAX); //Take account of voltage drop

        if (newVoltage != voltage)
        {
          voltage = newVoltage;
          normalizedVoltage = min(max(mapFloat(voltage, BATT_LOW, BATT_HIGH, 0.0f, 1.0f), 0.0f), 1.0f);
          onBatteryLevelUpdate();
        }


        if (voltage < BATT_MIN)
        {
          if (criticalLevelEnterTime == 0) criticalLevelEnterTime = millis();
          else if (millis() > criticalLevelEnterTime + CRITICAL_SHUT_TIME) onBatteryCriticalLevel();
        } else
        {
          criticalLevelEnterTime = 0;
        }
      }
    }

    void setCharging(bool value)
    {
      if (isCharging == value) return;

      isCharging = value;
      onChargingStateChanged();
    }

    typedef void(*onCriticalEvent)();
    void (*onBatteryCriticalLevel) ();

    void addBatteryCriticalLevelCallback (onCriticalEvent func) {
      onBatteryCriticalLevel = func;
    }

    static void criticalDefaultCallback()
    {
      //nothing
    }


    typedef void(*onChargingEvent)();
    void (*onChargingStateChanged) ();

    void addChargingStateChangedCallback (onChargingEvent func) {
      onChargingStateChanged = func;
    }

    static void chargingStateChangedDefaultCallback()
    {
      //nothing
    }

    typedef void(*onBatteryUpdateEvent)();
    void (*onBatteryLevelUpdate) ();

    void addBatteryLevelUpdateCallback (onBatteryUpdateEvent func) {
      onBatteryLevelUpdate = func;
    }

    static void batteryLevelUpdateDefaultCallback()
    {
      //nothing
    }

};
BatteryManager * BatteryManager::instance = nullptr;

#endif
