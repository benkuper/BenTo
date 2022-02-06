bool LedStripSystemLayer::initInternal()
{
    return true;
}

void LedStripSystemLayer::updateInternal()
{
    colors[numLeds-1] = Color(255, 0, 0, 255);
    //colors[1] = Color(0, 255, 0, 255);
    //colors[2] = Color(0, 0, 255, 255);
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