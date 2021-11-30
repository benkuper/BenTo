#pragma once

class LedStripBakeLayer : public LedStripLayer
{
public:
    LedStripBakeLayer(LedStripComponent *strip) : LedStripLayer("bake", LedStripLayer::BAKE, strip) {}
    ~LedStripBakeLayer() {}

    void initInternal() override;
    void updateInternal() override;
    void clearInternal() override;
};