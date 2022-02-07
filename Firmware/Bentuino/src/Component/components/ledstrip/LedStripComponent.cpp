bool LedStripComponent::initInternal()
{
    // init
    neoPixelStrip = NULL;
    dotStarStrip = NULL;

    for (int i = 0; i < LEDSTRIP_NUM_USER_LAYERS; i++)
        userLayers[i] = NULL;
    systemLayer = NULL;

    pin = GetIntConfig("pin");
    count = GetIntConfig("count");
    enPin = GetIntConfig("enPin");
    clkPin = GetIntConfig("clkPin");
    invertStrip = GetBoolConfig("invertStrip");

    if (enPin != 0)
    {
        NDBG("Setting Led Enable pin : " + String(enPin));
        pinMode(enPin, OUTPUT);
        digitalWrite(enPin, HIGH); // enable LEDs
    }

    brightness = addParameter("brightness", .5f, 0, 1);

    colors = (Color *)malloc(count * sizeof(Color));
    for (int i = 0; i < count; i++)
        colors[i] = Color(0, 0, 0, 0);

    if (count == 0 || pin == 0)
    {
        NDBG("Ledstrip pin and count have not been set");
        return false;
    }

    bakeLayer = addComponent(new LedStripBakeLayer(this));
    streamLayer = addComponent(new LedStripStreamLayer(this));
    scriptLayer = addComponent(new LedStripScriptLayer(this));
    systemLayer = addComponent(new LedStripSystemLayer(this));

    userLayers[0] = bakeLayer;
    userLayers[1] = streamLayer;
    userLayers[2] = scriptLayer;

    if (clkPin > 0)
    {
        dotStarStrip = new Adafruit_DotStar(count, pin, clkPin, DOTSTAR_BGR);
        dotStarStrip->begin();
        dotStarStrip->setBrightness(brightness->floatValue() * 255);
        //  dotStarStrip->fill(dotStarStrip->Color(255, 0, 0));
        //  dotStarStrip->show();
        //  delay(300);
    }
    else
    {
        neoPixelStrip = new Adafruit_NeoPixel(count, pin, NEO_GRB + NEO_KHZ800);
        neoPixelStrip->begin();
        neoPixelStrip->setBrightness(brightness->floatValue() * 255);
        //  neoPixelStrip->fill(neoPixelStrip->Color(255, 0, 0));
        //  neoPixelStrip->show();
        //  delay(300);
    }

    return true;
}

void LedStripComponent::updateInternal()
{
    // all layer's internal colors are updated in Component's update() function

    clearColors();
    for (int i = 0; i < LEDSTRIP_NUM_USER_LAYERS; i++)
        processLayer(userLayers[i]);

    processLayer(systemLayer);

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
}

void LedStripComponent::onEnabledChanged()
{
    if (enPin != 0)
        digitalWrite(enPin, enabled->intValue()); // enable LEDs
}

// Layer functions
void LedStripComponent::processLayer(LedStripLayer *layer)
{
    if (layer == NULL)
        return;

    if (!layer->enabled->boolValue())
        return;

    for (int i = 0; i < count; i++)
    {
        Color c = layer->colors[i];
        
        switch (layer->blendMode)
        {
        case LedStripLayer::ADD:
            colors[i] += c;
            //if(i == 0) NDBG(" > "+colors[i].toString());
            break;

        case LedStripLayer::MULTIPLY:
            colors[i] *= c;
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
    return invertStrip ? count - 1 - index : index;
}