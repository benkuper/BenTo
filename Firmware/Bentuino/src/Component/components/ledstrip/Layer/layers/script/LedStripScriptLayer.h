#pragma once

class LedStripScriptLayer : public LedStripLayer
{
public:
    LedStripScriptLayer(const String &name, LedStripComponent *strip) : LedStripLayer(name, LedStripLayer::ScriptType, strip) {}
    ~LedStripScriptLayer() {}

    Script script;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    virtual bool handleCommandInternal(const String &command, var *data, int numData);
};