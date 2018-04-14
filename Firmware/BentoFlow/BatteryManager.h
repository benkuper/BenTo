#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#define BATT_CHECK_INTERVAL 500

#define BATT_MIN 3.1
#define BATT_MAX 4.2
#define BATT_LOW 3.5
#define BATT_HIGH 3.9

#define CRITICAL_SHUT_TIME 10000 //shut off after 10s critical

class BatteryManager
{
  public:
    static BatteryManager * instance;

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

    }

    void update()
    {
      checkBattery();
    }

    float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    void checkBattery()
    {

      if (millis() - lastCheckTime > BATT_CHECK_INTERVAL)
      {
        lastCheckTime = millis();

        setCharging(analogRead(A0) >= 1020);

        float newVoltage = mapFloat(analogRead(A0), 750, 1000, BATT_MIN, BATT_MAX); //Take account of voltage drop


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
