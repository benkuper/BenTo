#include "LedStripComponent.h"
ImplementManagerSingleton(LedStrip)

    bool LedStripComponent::initInternal(JsonObject o)
{
    // init
    neoPixelStrip = NULL;
    dotStarStrip = NULL;

    for (int i = 0; i < LEDSTRIP_NUM_USER_LAYERS; i++)
        userLayers[i] = NULL;

    AddIntParam(count);
    AddIntParamConfig(dataPin);
    AddIntParamConfig(enPin);
    AddIntParamConfig(clkPin);
    AddFloatParam(brightness);
    AddBoolParamConfig(invertStrip);

#if USE_BAKELAYER
    AddOwnedComponent(&bakeLayer);
    userLayers[0] = &bakeLayer;
#endif
#if USE_STREAMINGLAYER
    AddOwnedComponent(&streamLayer);
    userLayers[1] = &streamLayer;
#endif
#if USE_SCRIPTLAYER
    AddOwnedComponent(&scriptLayer);
    userLayers[2] = &scriptLayer;
#endif
#if USE_SYSTEMLAYER
    AddOwnedComponent(&systemLayer);
#endif

    setupLeds();
    return true;
}

void LedStripComponent::setupLeds()
{
    if (!enabled)
        return;

    if (enPin != 0)
    {
        // NDBG("Setting Led Enable pin : " + String(enPin));
        pinMode(enPin, OUTPUT);
        digitalWrite(enPin, HIGH); // enable LEDs
    }

    if (count == 0 || dataPin == 0)
    {
        NDBG("Ledstrip pin and count have not been set");
        return;
    }

    // colors = (Color *)malloc(count * sizeof(Color));
    memset(colors, 0, LED_MAX_COUNT * sizeof(Color));
    for (int i = 0; i < count; i++)
        colors[i] = Color(0, 0, 0, 0);

    if (clkPin > 0)
    {
        NDBG("Setup dotstar");
        dotStarStrip = new Adafruit_DotStar(count, dataPin, clkPin, DOTSTAR_BGR);
        dotStarStrip->begin();
        dotStarStrip->setBrightness(brightness * LED_MAX_BRIGHTNESS * 255);
        // dotStarStrip->fill(dotStarStrip->Color(50, 100, 150));
        // dotStarStrip->show();
        // delay(300);
    }
    else
    {
        neoPixelStrip = new Adafruit_NeoPixel(count, dataPin, NEO_GRB + NEO_KHZ800);
        neoPixelStrip->begin();
        neoPixelStrip->setBrightness(brightness * LED_MAX_BRIGHTNESS * 255);
        // neoPixelStrip->fill(neoPixelStrip->Color(50, 100, 150));
        // neoPixelStrip->show();

        // delay(300);
    }

    setStripPower(true);
}

void LedStripComponent::updateInternal()
{

    if (dotStarStrip == NULL && neoPixelStrip == NULL)
    {
        return; // not active
    }

    // all layer's internal colors are updated in Component's update() function

    clearColors();
    for (int i = 0; i < LEDSTRIP_NUM_USER_LAYERS; i++)
        processLayer(userLayers[i]);

#if USE_SYSTEMLAYER
    processLayer(&systemLayer);
#endif

    showLeds();
}

void LedStripComponent::clearInternal()
{
    clearColors();
    showLeds();

    delete neoPixelStrip;
    neoPixelStrip = NULL;

    delete dotStarStrip;
    dotStarStrip = NULL;

    setStripPower(false);
}

void LedStripComponent::paramValueChangedInternal(void *param)
{
    if (param == &brightness)
    {
        if (neoPixelStrip != NULL)
            neoPixelStrip->setBrightness(brightness * LED_MAX_BRIGHTNESS * 255);
        else if (dotStarStrip != NULL)
            dotStarStrip->setBrightness(brightness * LED_MAX_BRIGHTNESS * 255);
    }
    else if (param == &count)
    {
        if (neoPixelStrip != NULL)
            neoPixelStrip->updateLength(count);
        else if (dotStarStrip != NULL)
            dotStarStrip->updateLength(count);
    }
}

void LedStripComponent::onEnabledChanged()
{
    setStripPower(enabled);
}

void LedStripComponent::setStripPower(bool value)
{
    DBG("Set Strip Power " + String(value));
    if (enPin != 0)
        digitalWrite(enPin, value); // enable LEDs

    pinMode(dataPin, value ? OUTPUT : OPEN_DRAIN);
}

// Layer functions
void LedStripComponent::processLayer(LedStripLayer *layer)
{
    if (layer == NULL)
        return;

    if (!layer->enabled)
        return;

    for (int i = 0; i < count; i++)
    {
        Color c = layer->colors[i];

        LedStripLayer::BlendMode bm = (LedStripLayer::BlendMode)layer->blendMode;

        switch (bm)
        {
        case LedStripLayer::Add:
            colors[i] += c;
            // if (i == 0)
            // NDBG(layer->name + " > " + colors[i].toString());
            break;

        case LedStripLayer::Multiply:
            colors[i] *= c;
            break;

        case LedStripLayer::Max:
            colors[i].r = max(colors[i].r, c.r);
            colors[i].g = max(colors[i].g, c.g);
            colors[i].b = max(colors[i].b, c.b);
            colors[i].a = max(colors[i].a, c.a);
            break;

        case LedStripLayer::Min:
            colors[i].r = min(colors[i].r, c.r);
            colors[i].g = min(colors[i].g, c.g);
            colors[i].b = min(colors[i].b, c.b);
            colors[i].a = min(colors[i].a, c.a);
            break;

        case LedStripLayer::Alpha:
        {
            float a = c.a / 255.0f;
            colors[i].r = colors[i].r + ((int)c.r - colors[i].r) * a;
            colors[i].g = colors[i].g + ((int)c.g - colors[i].g) * a;
            colors[i].b = colors[i].b + ((int)c.b - colors[i].b) * a;
            colors[i].a = max(colors[i].a, c.a);
        }

        default:
            break;

            break;
        }
    }
}

// Color functions

void LedStripComponent::clearColors()
{
    // for(int i=0;i<count;i++) colors[i] = Color(0,0,0,0);
    memset(colors, 0, sizeof(Color) * count);
}

void LedStripComponent::showLeds()
{
    if (neoPixelStrip != NULL)
    {
        for (int i = 0; i < count; i++)
        {
            float a = colors[i].a / 255.0f;
            neoPixelStrip->setPixelColor(ledMap(i),
                                         (uint8_t)(colors[i].r * a),
                                         (uint8_t)(colors[i].g * a),
                                         (uint8_t)(colors[i].b * a));
        }
        neoPixelStrip->show();
    }
    else if (dotStarStrip != NULL)
    {
        for (int i = 0; i < count; i++)
        {
            float a = colors[i].a / 255.0f;
            dotStarStrip->setPixelColor(ledMap(i),
                                        (uint8_t)(colors[i].r * a),
                                        (uint8_t)(colors[i].g * a),
                                        (uint8_t)(colors[i].b * a));
        }
        dotStarStrip->show();
    }
}

int LedStripComponent::ledMap(int index) const
{
    return (invertStrip ? count - 1 - index : index);
}
