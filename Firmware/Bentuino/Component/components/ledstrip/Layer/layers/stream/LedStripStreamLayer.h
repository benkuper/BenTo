#pragma once

class LedStripStreamLayer : public LedStripLayer
{
public:
    LedStripStreamLayer(LedStripComponent *strip) : LedStripLayer("stream", LedStripLayer::STREAM, strip) {}
    ~LedStripStreamLayer() {}

    void initInternal() override;
    void updateInternal() override;
    void clearInternal() override;
};