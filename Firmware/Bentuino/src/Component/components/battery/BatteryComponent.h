#pragma once
DeclareComponentSingleton(Battery, "battery",)

    bool initInternal(JsonObject o) override;

    void updateInternal() override;

    void clearInternal() override;

EndDeclareComponent