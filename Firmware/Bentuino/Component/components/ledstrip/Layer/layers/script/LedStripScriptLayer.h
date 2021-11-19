#pragma once

class LedStripScriptLayer : public LedStripLayer
{
public:
    LedStripScriptLayer(LedStripComponent *strip) : LedStripLayer("script", LedStripLayer::SCRIPT, strip) {}
    ~LedStripScriptLayer() {}

    void initInternal() override;
    void updateInternal() override;
    void clearInternal() override;
};