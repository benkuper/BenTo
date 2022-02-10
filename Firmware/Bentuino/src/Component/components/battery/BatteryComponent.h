#pragma once
DeclareComponentSingleton(Battery, "battery", )

    Parameter *sendFeedback;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

EndDeclareComponent