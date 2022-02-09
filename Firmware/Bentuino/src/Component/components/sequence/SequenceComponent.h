#pragma once
DeclareComponentSingleton(Sequence, "sequence",)

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent