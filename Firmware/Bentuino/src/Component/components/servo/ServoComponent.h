#pragma once
DeclareComponent(Servo, "servo",)

bool initInternal() override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent