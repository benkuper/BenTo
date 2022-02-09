#pragma once
DeclareComponent(Stepper, "stepper",)

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent