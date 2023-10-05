#pragma once
DeclareComponent(Stepper, "stepper", )

    static FastAccelStepperEngine engine;
static bool engineIsInit;
FastAccelStepper *stepper = NULL;

Parameter stepPin{"stepPin", 0, var(), var(), true};
Parameter dirPin{"dirPin", 0, var(), var(), true};
Parameter enPin{"enPin", 0, var(), var(), true};
Parameter accel{"accel", 0, var(), var(), true};

Parameter speed{"speed", 0.f};
Parameter position{"position",0.f};

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void onParameterEventInternal(const ParameterEvent &e) override;

EndDeclareComponent