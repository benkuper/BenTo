LedStripLayer::LedStripLayer(const String &name, Type t, LedStripComponent *strip) : Component(name),
                                                                                     strip(strip),
                                                                                     numLeds(strip->count),
                                                                                     type(t),
                                                                                     blendMode(ADD)
{
    colors = (Color *)malloc(numLeds * sizeof(Color));
    for (int i = 0; i < numLeds; i++)
        colors[i] = Color(0, 0, 0, 0);
}

LedStripLayer::~LedStripLayer()
{
    free(colors);
}
