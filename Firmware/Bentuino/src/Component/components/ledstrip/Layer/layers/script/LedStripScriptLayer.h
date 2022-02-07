#pragma once

class LedStripScriptLayer : public LedStripLayer
{
public:
    LedStripScriptLayer(LedStripComponent *strip) : LedStripLayer("script", LedStripLayer::Script, strip) {}
    ~LedStripScriptLayer() {}

    bool initInternal() override;
    void updateInternal() override;
    void clearInternal() override;
};