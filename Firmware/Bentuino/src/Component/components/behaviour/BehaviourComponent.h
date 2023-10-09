#pragma once

class BehaviourComponent : public Component
{
public:
    BehaviourComponent(bool enabled = true) : Component(getTypeString(), enabled)
    {
    }
    virtual String getTypeString() const override { return "behaviour"; }

    enum Comparator
    {
        EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        OPERATOR_MAX
    };

    const String operatorOptions[OPERATOR_MAX] = {
        "Equal",
        "Greater",
        "Greater or Equal",
        "Less",
        "Less or Equal"};

    DeclareConfigParameter(paramName, "");
    DeclareConfigParameter(comparator, EQUAL);
    DeclareConfigParameter(compareValue, 0.f);
    DeclareRangeConfigParameter(validationTime, 0.f, 0.f, 10.f);
    DeclareConfigParameter(alwaysTrigger, false);
    DeclareParameter(valid, false, false);

    enum Action
    {
        None,
        Shutdown,
        LaunchSeq,
        LaunchScript,
        LaunchCommand,
        ActionMax
    };

    const String triggerActionOptions[ActionMax] = {
        "None",
        "Shutdown",
        "Launch Sequence",
        "Launch Script",
        "Launch Command"
        };

    DeclareConfigParameter(triggerAction, None);
    DeclareConfigParameter(triggerValue, "");

    Parameter *targetParam;
    int listenerIndex = -1;
    float timeAtValidation = 0.f;
    bool delayedValidation = false;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;

    void updateTargetParameter();

    void onParameterEventInternal(const ParameterEvent &e) override;

    void onTargetParameterChanged(const ParameterEvent &e);

    void trigger();

    DeclareComponentEventTypes(CommandLaunched);
    DeclareComponentEventNames("CommandLaunched");
};