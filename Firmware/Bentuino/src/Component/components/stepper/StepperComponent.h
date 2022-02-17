#pragma once
class StepperComponent : public Component
{
public:
    StepperComponent() : Component(Type_Stepper) {}
    ~StepperComponent() {}

    static FastAccelStepperEngine engine;
    static bool engineIsInit;
    FastAccelStepper *stepper = NULL;

    Parameter *stepPin;
    Parameter *dirPin;
    Parameter *enPin;
    Parameter *speed;
    Parameter *accel;

    Parameter *position;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    void onParameterEventInternal(const ParameterEvent &e) override;
};