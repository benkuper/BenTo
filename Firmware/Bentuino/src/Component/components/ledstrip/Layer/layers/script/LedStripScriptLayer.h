#pragma once

class LedStripScriptLayer : public LedStripLayer
{
public:
    LedStripScriptLayer(LedStripComponent *strip) : LedStripLayer("script", LedStripLayer::ScriptType, strip, Type_ScriptLayer), script(this) {}
    ~LedStripScriptLayer() {}

    Script script;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    virtual bool handleCommandInternal(const String &command, var *data, int numData);
};