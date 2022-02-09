#pragma once
DeclareComponent(Servo, "servo",)

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent