#pragma once

class LedStripStreamLayer : public LedStripLayer
{
public:
    LedStripStreamLayer(LedStripComponent *strip) : LedStripLayer("stream", LedStripLayer::STREAM, strip) {}
    ~LedStripStreamLayer() {}

    bool initInternal() override;
    void updateInternal() override;
    void clearInternal() override;
};