#pragma once

class LedStripBakeLayer : public LedStripLayer
{
public:
    LedStripBakeLayer(LedStripComponent *strip) : LedStripLayer("bake", LedStripLayer::BAKE, strip) {}
    ~LedStripBakeLayer() {}

    bool initInternal() override;
    void updateInternal() override;
    void clearInternal() override;
};