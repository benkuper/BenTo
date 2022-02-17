#pragma once

class ServoComponent : public Component
{
public:
    ServoComponent() : Component(Type_Servo) {}
    ~ServoComponent() {}

    Parameter *pin;
    Parameter *position;

    Servo servo;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    void onParameterEventInternal(const ParameterEvent &e) override;
};