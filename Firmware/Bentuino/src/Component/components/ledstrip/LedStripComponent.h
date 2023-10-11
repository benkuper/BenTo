#pragma once

#define LEDSTRIP_NUM_USER_LAYERS 3
#define USE_BAKELAYER 0

#ifdef USE_STREAMING
#define USE_STREAMINGLAYER 1
#else
#define USE_STREAMINGLAYER 0
#endif

#ifdef USE_SCRIPT
#define USE_SCRIPTLAYER 0
#else
#define USE_SCRIPTLAYER 0
#endif

#define USE_SYSTEMLAYER 1

class LedStripComponent : public Component
{
public:
    LedStripComponent(const String& name = "strip", bool enabled = true) : Component(name, enabled)
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
                                             scriptLayer(this)
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
    LedStripScriptLayer scriptLayer;
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
    

    void paramValueChanged(void *param) override;
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
    FillOSCQueryFloatParam(brightness);
    FillOSCQueryBoolParam(invertStrip);
    FillOSCQueryInternalEnd
};

DeclareComponentManager(LedStrip, LEDSTRIP, strips, strip)
    EndDeclareComponent