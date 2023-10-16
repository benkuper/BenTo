#pragma once

class LedStripComponent;

class LedStripLayer : public Component
{
public:
    enum Type
    {
        System,
        Stream,
        Bake,
        ScriptType // need to not be "Script" to avoid Class confusion
    };

    enum BlendMode
    {
        Add,
        Multiply,
        Max,
        Min,
        Alpha,
        BlendModeMax
    };

    LedStripLayer(const String &name, Type t, LedStripComponent *strip);
    ~LedStripLayer();

    virtual bool initInternal(JsonObject o) override;

    LedStripComponent *strip;
    Type type;

    DeclareIntParam(blendMode, Add);
    const String blendModeOptions[BlendModeMax] = {"Add", "Multiply", "Max", "Min", "Alpha"};

    Color colors[LED_MAX_COUNT];

    // Helper functions
    void clearColors();
    void fillAll(Color c);
    void fillRange(Color c, float start, float end, bool clear = true);
    void point(Color c, float pos, float radius, bool clear = true);
    void setLed(int index, Color c);
    Color getLed(int index);

    void setBlendMode(BlendMode b);

    HandleSetParamInternalStart
        CheckAndSetEnumParam(blendMode, blendModeOptions, BlendModeMax);
    HandleSetParamInternalEnd;

    FillSettingsInternalStart
        FillSettingsParam(blendMode);
    FillSettingsInternalEnd;

    FillOSCQueryInternalStart
            FillOSCQueryEnumParam(blendMode, blendModeOptions, BlendModeMax);
    FillOSCQueryInternalEnd
};