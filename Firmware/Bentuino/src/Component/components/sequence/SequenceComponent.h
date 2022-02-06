#pragma once
DeclareComponentSingleton(Sequence, "sequence",)

bool initInternal() override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent