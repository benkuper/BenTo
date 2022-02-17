#pragma once

#define LEDSTRIP_NUM_USER_LAYERS 3

class LedStripComponent : public Component
{
public:
    LedStripComponent();
    ~LedStripComponent() {}

    Parameter *dataPin;
    Parameter *count;
    Parameter *clkPin;
    Parameter *enPin;

    Parameter *brightness;

    // mapping
    Parameter *invertStrip;

    Color colors[MAX_PIXELS];

    LedStripBakeLayer bakeLayer;
    LedStripStreamLayer streamLayer;
    LedStripScriptLayer scriptLayer;

    LedStripSystemLayer systemLayer;

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
};