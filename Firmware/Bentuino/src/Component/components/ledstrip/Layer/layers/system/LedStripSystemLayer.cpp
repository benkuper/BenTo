bool LedStripSystemLayer::initInternal()
{
    return true;
}

void LedStripSystemLayer::updateInternal()
{
    colors[0] = Color(0,100,0);
    colors[numLeds - 1] = Color::HSV(cosf(millis() / 1000.f) * .5f + .5f, 1, 1);
    colors[numLeds / 2] = Color(255, 0, 255);
}

void LedStripSystemLayer::clearInternal()
{
}

void LedStripSystemLayer::setSystemMode(SystemMode m)
{
    if (systemMode == m)
        return;
    systemMode = m;
    timeAtModeChanged = millis();
}

void LedStripSystemLayer::setConnectionMode(SystemConnectionMode m)
{
    if (connectionMode == m)
        return;
    connectionMode = m;
    timeAtConnectionModeChanged = millis();
}