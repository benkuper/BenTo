#pragma once

class LedStripSystemLayer : public LedStripLayer
{
public:
    LedStripSystemLayer(LedStripComponent *strip) : LedStripLayer("system", LedStripLayer::System, strip) {}
    ~LedStripSystemLayer() {}

    bool initInternal() override;
    void updateInternal() override;
    void clearInternal() override;

    void updateWifiStatus();
    void updateShutdown();
};