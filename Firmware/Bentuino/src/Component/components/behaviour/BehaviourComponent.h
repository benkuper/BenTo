#pragma once

DeclareComponent(Behaviour, "behaviour", )

    enum Comparator {
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

DeclareStringParam(paramName, "");
DeclareIntParam(comparator, EQUAL);
DeclareFloatParam(compareValue, 0.f);
DeclareFloatParam(validationTime, 0.f);
DeclareBoolParam(alwaysTrigger, false);
DeclareBoolParam(valid, false);

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
    "Launch Command"};

DeclareIntParam(triggerAction, None);
DeclareStringParam(triggerValue, "");

void *targetParam;
int listenerIndex = -1;
float timeAtValidation = 0.f;
bool delayedValidation = false;

bool initInternal(JsonObject o) override;
void updateInternal() override;

void updateTargetParameter();

// void onParameterEventInternal(const ParameterEvent &e) override;

// void onTargetParameterChanged(const ParameterEvent &e);

void trigger();

DeclareComponentEventTypes(CommandLaunched);
DeclareComponentEventNames("CommandLaunched");

HandleSetParamInternalStart
    CheckAndSetParam(paramName);
CheckAndSetEnumParam(comparator, operatorOptions, OPERATOR_MAX);
CheckAndSetParam(compareValue);
CheckAndSetParam(validationTime);
CheckAndSetParam(alwaysTrigger);
CheckAndSetEnumParam(triggerAction, triggerActionOptions, ActionMax);
CheckAndSetParam(triggerValue);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(paramName);
FillSettingsParam(comparator);
FillSettingsParam(compareValue);
FillSettingsParam(validationTime);
FillSettingsParam(alwaysTrigger);
FillSettingsParam(triggerAction);
FillSettingsParam(triggerValue);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryStringParam(paramName);
FillOSCQueryEnumParam(comparator, operatorOptions, OPERATOR_MAX);
FillOSCQueryFloatParam(compareValue);
FillOSCQueryFloatParam(validationTime);
FillOSCQueryBoolParam(alwaysTrigger);
FillOSCQueryBoolParamReadOnly(valid);
FillOSCQueryEnumParam(triggerAction, triggerActionOptions, ActionMax);
FillOSCQueryStringParam(triggerValue);
FillOSCQueryInternalEnd;

};

DeclareComponentManager(Behaviour, BEHAVIOUR, behaviours, behaviour)

      void addItemInternal(int index) {  };
    void onChildComponentEvent(const ComponentEvent &e) override;
DeclareComponentEventTypes(CommandLaunched);
DeclareComponentEventNames("CommandLaunched");
EndDeclareComponent