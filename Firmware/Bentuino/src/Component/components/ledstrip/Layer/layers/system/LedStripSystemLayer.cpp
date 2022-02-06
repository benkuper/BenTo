bool LedStripSystemLayer::initInternal()
{
    return true;
}

void LedStripSystemLayer::updateInternal()
{
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