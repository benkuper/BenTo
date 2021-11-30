LedStripLayer::LedStripLayer(const String &name, Type t, LedStripComponent *strip) : Component(name),
                                                                                     strip(strip),
                                                                                     numLeds(strip->count->val),
                                                                                     type(t),
                                                                                     blendMode(ADD)
{
    colors = (Color *)malloc(numLeds * sizeof(Color));
}

LedStripLayer::~LedStripLayer()
{
    free(colors);
}
