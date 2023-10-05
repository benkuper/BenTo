#pragma once

class LedStripSystemLayer : public LedStripLayer
{
public:
    LedStripSystemLayer(LedStripComponent *strip) : LedStripLayer("systemLayer", LedStripLayer::System, strip) {}
    ~LedStripSystemLayer() {}

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    void updateWifiStatus();
    void updateShutdown();
};