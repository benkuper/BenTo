#pragma once

class LedStripSystemLayer : public LedStripLayer
{
public:
    LedStripSystemLayer(const String &name, LedStripComponent *strip) : LedStripLayer(name, LedStripLayer::System, strip) {}
    ~LedStripSystemLayer() {}

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    void updateWifiStatus();
    void updateShutdown();
};