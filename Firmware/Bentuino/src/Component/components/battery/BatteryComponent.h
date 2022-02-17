#pragma once

class BatteryComponent : public Component
{
public:
    BatteryComponent() : Component(Type_Battery) { instance = this; }
    ~BatteryComponent() {}

    DeclareSingleton(BatteryComponent);

    Parameter *pin;
    Parameter *chargePin;
    Parameter *rawValue;
    Parameter *minRaw;
    Parameter *maxRaw;
    Parameter *value;
    Parameter *voltage;
    Parameter *lowBattery;
    Parameter *isCharging;
    Parameter *readRate;

    const long lowBatteryShutdownTime = 5000;
    long timeAtLowBattery;

    long timeSinceLastBatteryRead;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    void onParameterEventInternal(const ParameterEvent &e) override;

    DeclareComponentEventTypes(CriticalBattery);
    DeclareComponentEventNames("CriticalBattery");
};