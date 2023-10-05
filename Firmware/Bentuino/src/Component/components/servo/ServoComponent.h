#pragma once

DeclareComponent(Servo, "servo", )

Parameter pin{"pin", 0, var(), var(), true};
Parameter position{"position", 0.f, 0.f, 1.f};

Servo servo;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void onParameterEventInternal(const ParameterEvent &e) override;

EndDeclareComponent