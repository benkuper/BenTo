#pragma once

#define LEDSTRIP_NUM_USER_LAYERS 3

DeclareComponent(LedStrip, "ledstrip", )

Parameter<int> *pin;
Parameter<int> *count;

Color *colors;

// user layers, may be more than one later
LedStripBakeLayer *bakeLayer;
LedStripStreamLayer *streamLayer;
LedStripScriptLayer *scriptLayer;

// on top of all
LedStripSystemLayer *systemLayer;

LedStripLayer *userLayers[LEDSTRIP_NUM_USER_LAYERS];

Adafruit_NeoPixel *strip;

void initInternal() override
{
    count = addParameter<int>("count", 32);
    pin = addParameter<int>("pin", 27);

    strip = new Adafruit_NeoPixel(count->val, pin->val, NEO_GRB + NEO_KHZ800);

    colors = (Color *)malloc(count->val * sizeof(Color));

    bakeLayer = addComponent(new LedStripBakeLayer(this));
    streamLayer = addComponent(new LedStripStreamLayer(this));
    scriptLayer = addComponent(new LedStripScriptLayer(this));
    systemLayer = addComponent(new LedStripSystemLayer(this));

    userLayers[0] = bakeLayer;
    userLayers[1] = streamLayer;
    userLayers[2] = scriptLayer;
}

void updateInternal()
{
    // all layer's internal colors are updated in Component's update() function

    for (int i = 0; i < LEDSTRIP_NUM_USER_LAYERS; i++)
        processLayer(userLayers[i]);

    processLayer(systemLayer);

    showLeds();
}

void clearInternal()
{
    clearColors();
    showLeds();

    delete strip;
    strip = NULL;
}

// Layer functions
void processLayer(LedStripLayer *layer)
{
    int numLeds = count->val;
    for (int i = 0; i < numLeds; i++)
    {
        Color c = layer->colors[i];
        switch (layer->blendMode)
        {
        case LedStripLayer::ADD:
            colors[i].r = min(colors[i].r + c.r, 255);
            colors[i].g = min(colors[i].g + c.g, 255);
            colors[i].b = min(colors[i].b + c.b, 255);
            colors[i].a = min(colors[i].a + c.a, 255);
            break;

        case LedStripLayer::MULTIPLY:
            colors[i].r *= c.r / 255.0f;
            colors[i].g *= c.g / 255.0f;
            colors[i].b *= c.b / 255.0f;
            colors[i].a *= c.a / 255.0f;
            break;

        case LedStripLayer::MAX:
            colors[i].r = max(colors[i].r, c.r);
            colors[i].g = max(colors[i].g, c.g);
            colors[i].b = max(colors[i].b, c.b);
            colors[i].a = max(colors[i].a, c.a);
            break;

        case LedStripLayer::MIN:
            colors[i].r = min(colors[i].r, c.r);
            colors[i].g = min(colors[i].g, c.g);
            colors[i].b = min(colors[i].b, c.b);
            colors[i].a = min(colors[i].a, c.a);
            break;

        case LedStripLayer::ALPHA:
        {
            float a = c.a / 255.0f;
            colors[i].r = colors[i].r + (c.r - colors[i].r) * a;
            colors[i].g = colors[i].g + (c.g - colors[i].g) * a;
            colors[i].b = colors[i].b + (c.b - colors[i].b) * a;
            colors[i].r = max(colors[i].a, c.a);
        }
        break;
        }
    }
}

// Color functions

inline void clearColors()
{
    memset(colors, 0, count->val * sizeof(Color));
}

inline void showLeds()
{
    if(strip != NULL) strip->show();
}

EndDeclareComponent