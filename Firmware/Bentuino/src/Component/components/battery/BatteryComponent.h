#pragma once
DeclareComponentSingleton(Battery, "battery",)

    bool initInternal() override;

    void updateInternal() override;

    void clearInternal() override;

EndDeclareComponent