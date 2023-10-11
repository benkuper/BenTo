#pragma once

#define LEDSTRIP_NUM_USER_LAYERS 3
#define USE_BAKELAYER 1

#ifdef USE_STREAMING
#define USE_STREAMINGLAYER 1
#else
#define USE_STREAMINGLAYER 0
#endif

#ifdef USE_SCRIPT
#define USE_SCRIPTLAYER 1
#else
#define USE_SCRIPTLAYER 0
#endif

#define USE_SYSTEMLAYER 1

class LedStripComponent : public Component
{
public:
    LedStripComponent(const String &name = "strip", bool enabled = true) : Component(name, enabled)
#if USE_BAKELAYER
                                                                           ,
                                                                           bakeLayer(this)
#endif
#if USE_STREAMINGLAYER
                                                                           ,
                                                                           streamLayer(this)
#endif
#if USE_SCRIPTLAYER
                                                                           ,
                                                                           scriptLayer("script", LedStripLayer::Type::ScriptType, this)
#endif
#if USE_SYSTEMLAYER
                                                                           ,
                                                                           systemLayer(this)
#endif
    {
    }

    ~LedStripComponent() {}
    virtual String getTypeString() const override { return "ledstrip"; }

    DeclareIntParam(count, LED_DEFAULT_COUNT);
    DeclareIntParam(dataPin, LED_DEFAULT_DATA_PIN);
    DeclareIntParam(clkPin, LED_DEFAULT_CLK_PIN);
    DeclareIntParam(enPin, LED_DEFAULT_EN_PIN);

    DeclareFloatParam(brightness, LED_DEFAULT_BRIGHTNESS);

    // mapping
    DeclareBoolParam(invertStrip, LED_DEFAULT_INVERT_DIRECTION);

    Color colors[LED_MAX_COUNT];

    // user layers, may be more than one later
#if USE_BAKELAYER
    LedStripBakeLayer bakeLayer;
#endif
#if USE_STREAMINGLAYER
    LedStripStreamLayer streamLayer;
#endif
#if USE_SCRIPTLAYER
    LedStripLayer scriptLayer;
#endif
#if USE_SYSTEMLAYER
    LedStripSystemLayer systemLayer;
#endif

    LedStripLayer *userLayers[LEDSTRIP_NUM_USER_LAYERS];

    Adafruit_NeoPixel *neoPixelStrip;
    Adafruit_DotStar *dotStarStrip;

    bool initInternal(JsonObject o) override;

    void setupLeds();

    void updateInternal() override;
    void clearInternal() override;

    void paramValueChangedInternal(void *param) override;
    void onEnabledChanged() override;

    void setStripPower(bool value);

    // Layer functions
    void processLayer(LedStripLayer *layer);

    // Color functions
    void clearColors();
    void showLeds();

    int ledMap(int index) const;

    HandleSetParamInternalStart
        CheckAndSetParam(count);
    CheckAndSetParam(dataPin);
    CheckAndSetParam(clkPin);
    CheckAndSetParam(enPin);
    CheckAndSetParam(brightness);
    CheckAndSetParam(invertStrip);
    HandleSetParamInternalEnd;

    FillSettingsInternalStart
        FillSettingsParam(count);
    FillSettingsParam(dataPin);
    FillSettingsParam(clkPin);
    FillSettingsParam(enPin);
    FillSettingsParam(brightness);
    FillSettingsParam(invertStrip);
    FillSettingsInternalEnd

        FillOSCQueryInternalStart
            FillOSCQueryIntParam(count);
    FillOSCQueryIntParam(dataPin);
    FillOSCQueryIntParam(clkPin);
    FillOSCQueryIntParam(enPin);
    FillOSCQueryRangeParam(brightness, 0, 1);
    FillOSCQueryBoolParam(invertStrip);
    FillOSCQueryInternalEnd
};


DeclareComponentManager(LedStrip, LEDSTRIP, leds, strip)

#ifdef USE_SCRIPT

    LinkScriptFunctionsStart
    LinkScriptFunction(LedStripManagerComponent, clear, v, );
LinkScriptFunction(LedStripManagerComponent, fillAll, v, i);
LinkScriptFunction(LedStripManagerComponent, fillRange, v, iff);
LinkScriptFunction(LedStripManagerComponent, fillRGB, v, iii);
LinkScriptFunction(LedStripManagerComponent, fillHSV, v, fff);

LinkScriptFunction(LedStripManagerComponent, point, v, iff);
LinkScriptFunction(LedStripManagerComponent, pointRGB, v, iiiff);
LinkScriptFunction(LedStripManagerComponent, pointHSV, v, fffff);

LinkScriptFunction(LedStripManagerComponent, set, v, ii);
LinkScriptFunction(LedStripManagerComponent, get, i, i);
LinkScriptFunction(LedStripManagerComponent, setRGB, v, iiii);
LinkScriptFunction(LedStripManagerComponent, setHSV, v, ifff);

LinkScriptFunctionsEnd

DeclareScriptFunctionVoid0(LedStripManagerComponent, clear) { items[0]->scriptLayer.clearColors(); }
DeclareScriptFunctionVoid1(LedStripManagerComponent, fillAll, uint32_t) { items[0]->scriptLayer.fillAll(arg1); }
DeclareScriptFunctionVoid3(LedStripManagerComponent, fillRange, uint32_t, float, float) { items[0]->scriptLayer.fillRange(arg1, arg2, arg3); }

DeclareScriptFunctionVoid3(LedStripManagerComponent, fillRGB, uint32_t, uint32_t, uint32_t) { items[0]->scriptLayer.fillAll(Color(arg1, arg2, arg3)); }
DeclareScriptFunctionVoid3(LedStripManagerComponent, fillHSV, float, float, float) { items[0]->scriptLayer.fillAll(Color::HSV(arg1, arg2, arg3)); }

DeclareScriptFunctionVoid3(LedStripManagerComponent, point, uint32_t, float, float) { items[0]->scriptLayer.point(arg1, arg2, arg3, false); }
DeclareScriptFunctionVoid5(LedStripManagerComponent, pointRGB, uint32_t, uint32_t, uint32_t, float, float) { items[0]->scriptLayer.point(Color(arg1, arg2, arg3), arg4, arg5, false); }
DeclareScriptFunctionVoid5(LedStripManagerComponent, pointHSV, float, float, float, float, float) { items[0]->scriptLayer.point(Color::HSV(arg1, arg2, arg3), arg4, arg5, false); }

DeclareScriptFunctionVoid2(LedStripManagerComponent, set, uint32_t, uint32_t) { items[0]->scriptLayer.setLed(arg1, arg2); }
DeclareScriptFunctionVoid4(LedStripManagerComponent, setRGB, uint32_t, uint32_t, uint32_t, uint32_t) { items[0]->scriptLayer.setLed(arg1, Color(arg2, arg3, arg4)); }
DeclareScriptFunctionVoid4(LedStripManagerComponent, setHSV, uint32_t, float, float, float) { items[0]->scriptLayer.setLed(arg1, Color::HSV(arg2, arg3, arg4)); }
DeclareScriptFunctionReturn1(LedStripManagerComponent, get, uint32_t, uint32_t) { return items[0]->scriptLayer.getLed(arg1).value; }

#endif

EndDeclareComponent