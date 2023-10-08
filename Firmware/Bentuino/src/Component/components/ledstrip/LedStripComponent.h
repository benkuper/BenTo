#pragma once

#define LEDSTRIP_NUM_USER_LAYERS 3
#define USE_BAKELAYER 1
#define USE_STREAMLAYER 1
#define USE_SCRIPTLAYER 1
#define USE_SYSTEMLAYER 1

// DeclareComponent(LedStrip, "ledstrip", )
class LedStripComponent : public Component
{
public:
    LedStripComponent(const String &name = "ledstrip", bool enabled = true) : Component(getTypeString(), enabled)
#if USE_BAKELAYER
                                                                              ,
                                                                              bakeLayer(this)
#endif
#if USE_STREAMLAYER
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

    Parameter count{"count", LED_DEFAULT_COUNT, var(), var(), true};
    Parameter dataPin{"dataPin", LED_DEFAULT_DATA_PIN, var(), var(), true};
    Parameter clkPin{"clkPin", LED_DEFAULT_CLK_PIN, var(), var(), true};
    Parameter enPin{"enPin", LED_DEFAULT_EN_PIN, var(), var(), true};

    Parameter brightness{"brightness", LED_DEFAULT_BRIGHTNESS, 0, 1, true};

    // mapping
    Parameter invertStrip{"invertStrip", LED_DEFAULT_INVERT_DIRECTION, var(), var(), true};


    Color colors[LED_MAX_COUNT];

    // user layers, may be more than one later
#if USE_BAKELAYER
    LedStripBakeLayer bakeLayer;
#endif
#if USE_STREAMLAYER
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
    void updateInternal() override;
    void clearInternal() override;

    void onParameterEventInternal(const ParameterEvent &e) override;
    void onEnabledChanged() override;

    void setStripPower(bool value);

    // Layer functions
    void processLayer(LedStripLayer *layer);

    // Color functions
    void clearColors();
    void showLeds();

    int ledMap(int index) const;

    EndDeclareComponent