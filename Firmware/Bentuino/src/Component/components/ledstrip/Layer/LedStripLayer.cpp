LedStripLayer::LedStripLayer(const String &name, Type t, LedStripComponent *strip) : Component(name),
                                                                                     strip(strip),
                                                                                     type(t),
                                                                                     colors(nullptr)
{
    blendMode = addParameter("blendMode", Add);
    initColors();
}

LedStripLayer::~LedStripLayer()
{
    free(colors);
}

void LedStripLayer::initColors()
{
    if (colors != nullptr)
        free(colors);

    numLeds = strip->count->intValue();
    colors = (Color *)malloc(numLeds * sizeof(Color));
    for (int i = 0; i < numLeds; i++)
        colors[i] = Color(0, 0, 0, 0);
}

// Helpers

void LedStripLayer::clearColors()
{
    // for(int i=0;i<count;i++) colors[i] = Color(0,0,0,0);
    memset(colors, 0, sizeof(Color) * numLeds);
}

void LedStripLayer::fillAll(Color c)
{
    fillRange(c, 0, 1);
}

void LedStripLayer::fillRange(Color c, float start, float end, bool doClear)
{
    if (doClear)
        clearColors();

    int s = max(min(start, end), 0.f) * (numLeds - 1);
    int e = min(max(start, end), 1.f) * (numLeds - 1);

    for (int i = s; i <= e; i++)
    {
        colors[i] += c;
    }
}

void LedStripLayer::point(Color c, float pos, float radius, bool doClear)
{
    if (doClear)
        clearColors();
    if (radius == 0)
        return;

    for (int i = 0; i < numLeds; i++)
    {
        float rel = i * 1.0f / max(numLeds - 1, 1);
        float fac = max(1 - (std::abs((float)(pos - rel)) / radius), 0.f);
        Color tc = c.withMultipliedAlpha(fac);
        colors[i] += tc;
    }
}