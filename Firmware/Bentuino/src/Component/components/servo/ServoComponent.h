#pragma once

DeclareComponent(Servo, "servo",)

Parameter * pin;
Parameter * position;

Servo servo;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void onParameterEventInternal(const ParameterEvent &e) override;

EndDeclareComponent