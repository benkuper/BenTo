#pragma once

class LedStripSystemLayer : public LedStripLayer
{
public:
    LedStripSystemLayer(LedStripComponent *strip) : LedStripLayer("system", LedStripLayer::SYSTEM, strip) {}
    ~LedStripSystemLayer() {}

    bool initInternal() override;
    void updateInternal() override;
    void clearInternal() override;
};