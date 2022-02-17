LedStripComponent::LedStripComponent() : Component(Type_Strip, "strip"),
                                         bakeLayer(this),
                                         streamLayer(this),
                                         scriptLayer(this),
                                         systemLayer(this)
{
}

bool LedStripComponent::initInternal(JsonObject o)
{
    // init
    neoPixelStrip = NULL;
    dotStarStrip = NULL;

    dataPin = AddConfigParameter("dataPin", 0);
    count = AddRangeConfigParameter("count", 0, 0, MAX_PIXELS);
    enPin = AddConfigParameter("enPin", 0);
    clkPin = AddConfigParameter("clkPin", 0);
    invertStrip = AddConfigParameter("invertStrip", false);

    if (enPin->intValue() != 0)
    {
        NDBG("Setting Led Enable pin : " + String(enPin->intValue()));
        pinMode(enPin->intValue(), OUTPUT);
        digitalWrite(enPin->intValue(), HIGH); // enable LEDs
    }

    brightness = addParameter("brightness", .5f, 0, 1);

    // colors = (Color *)malloc(count->intValue() * sizeof(Color));
    memset(colors, 0, sizeof(Color) * MAX_PIXELS);

    if (count->intValue() == 0 || dataPin->intValue() == 0)
    {
        NDBG("Ledstrip pin and count have not been set");
        return false;
    }

    AddComponent(bakeLayer, "bake");
    AddComponent(streamLayer, "stream");
    AddComponent(scriptLayer, "script");
    AddComponent(systemLayer, "system");

    userLayers[0] = &bakeLayer;
    userLayers[1] = &streamLayer;
    userLayers[2] = &scriptLayer;

    if (clkPin->intValue() > 0)
    {
        NDBG("Using dotStar lib");
        dotStarStrip = new Adafruit_DotStar(count->intValue(), dataPin->intValue(), clkPin->intValue(), DOTSTAR_BGR);
        dotStarStrip->begin();
        dotStarStrip->setBrightness(brightness->floatValue() * 255);
         dotStarStrip->fill(dotStarStrip->Color(255, 0, 0));
         dotStarStrip->show();
         delay(300);
    }
    else
    {
        NDBG("Using neopixel lib");
        neoPixelStrip = new Adafruit_NeoPixel(count->intValue(), dataPin->intValue(), NEO_GRB + NEO_KHZ800);
        neoPixelStrip->begin();
        neoPixelStrip->setBrightness(brightness->floatValue() * 255);
         neoPixelStrip->fill(neoPixelStrip->Color(20, 0, 0));
         neoPixelStrip->show();
         delay(300);
    }

    return true;
}

void LedStripComponent::updateInternal()
{
    if(!enabled->boolValue()) return;

    if (dotStarStrip == NULL && neoPixelStrip == NULL)
    {
        return; // not active
    }

    // all layer's internal colors are updated in Component's update() function

    clearColors();
    for (int i = 0; i < LEDSTRIP_NUM_USER_LAYERS; i++)
        processLayer(userLayers[i]);

    processLayer(&systemLayer);

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

void LedStripComponent::onParameterEventInternal(const ParameterEvent &e)
{
    if (e.parameter == brightness)
    {
        if (neoPixelStrip != NULL)
            neoPixelStrip->setBrightness(brightness->floatValue() * 255);
        else if (dotStarStrip != NULL)
            dotStarStrip->setBrightness(brightness->floatValue() * 255);
    }
    else if (e.parameter == count)
    {
        if (neoPixelStrip != NULL)
            neoPixelStrip->updateLength(count->intValue());
        else if (dotStarStrip != NULL)
            dotStarStrip->setBrightness(count->intValue());
    }
}

void LedStripComponent::onEnabledChanged()
{
    setStripPower(enabled->boolValue());
}

void LedStripComponent::setStripPower(bool value)
{
    if (enPin->intValue() != 0)
        digitalWrite(enPin->intValue(), value); // enable LEDs

    pinMode(dataPin->intValue(), value ? OUTPUT : OPEN_DRAIN);
}

// Layer functions
void LedStripComponent::processLayer(LedStripLayer *layer)
{
    if (layer == NULL)
        return;

    if (!layer->enabled->boolValue())
        return;

    for (int i = 0; i < count->intValue(); i++)
    {
        Color c = layer->colors[i];

        LedStripLayer::BlendMode bm = (LedStripLayer::BlendMode)layer->blendMode->intValue();

        switch (bm)
        {
        case LedStripLayer::Add:
            colors[i] += c;
            // if(i == 0) NDBG(" > "+colors[i].toString());
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

void LedStripComponent::clearColors()
{
    // for(int i=0;i<count->intValue();i++) colors[i] = Color(0,0,0,0);
    memset(colors, 0, sizeof(Color) * count->intValue());
}

void LedStripComponent::showLeds()
{
    if (neoPixelStrip != NULL)
    {
        for (int i = 0; i < count->intValue(); i++)
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
        for (int i = 0; i < count->intValue(); i++)
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
    return invertStrip->intValue() ? count->intValue() - 1 - index : index;
}