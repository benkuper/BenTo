#pragma once

class LedStripScriptLayer : public LedStripLayer
{
public:
    LedStripScriptLayer(const String &name, LedStripComponent *strip) : LedStripLayer(name, LedStripLayer::Script, strip) {}
    ~LedStripScriptLayer() {}

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;
};