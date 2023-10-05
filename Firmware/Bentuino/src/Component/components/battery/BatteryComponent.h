#pragma once
DeclareComponentSingleton(Battery, "battery", )

DeclareConfigParameter(sendFeedback, true);

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent