// #pragma once

// class LedStripScriptLayer : public LedStripLayer
// {
// public:
//     LedStripScriptLayer(LedStripComponent *strip) : LedStripLayer("scriptLayer", LedStripLayer::ScriptType, strip),script(this) {}
//     ~LedStripScriptLayer() {}

//     bool initInternal(JsonObject o) override;
//     void updateInternal() override;
//     void clearInternal() override;

//     // virtual bool handleCommandInternal(const String &command, var *data, int numData);
// };