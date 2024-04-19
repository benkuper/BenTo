bool LedStripSystemLayer::initInternal(JsonObject o)
{
    LedStripLayer::initInternal(o);
    blendMode = BlendMode::Alpha;
    return true;
}

void LedStripSystemLayer::updateInternal()
{

    clearColors();
    updateWifiStatus();
    updateShutdown();
}

void LedStripSystemLayer::clearInternal()
{
    clearColors();
}

void LedStripSystemLayer::updateWifiStatus()
{
    if (WifiComponent::instance == NULL)
        return;

    float relT = (millis() - WifiComponent::instance->timeAtStateChange) / 1000.0f;
    const float animTime = 1.0f;

    WifiComponent::ConnectionState connectionState = WifiComponent::instance->state;

    // fillRange(Color(255, 255, 0), .25, .5f, false); // clear strip

    if (connectionState != WifiComponent::Connecting && relT > animTime)
        return;

    Color color = BatteryComponent::instance->getBatteryColor();

    // NDBG("Wifi status : " + String(connectionState) + " " + String(relT));

    // default behavior (connecting) on which we will add animation for connected behavior
    float t = (millis() - RootComponent::instance->timeAtStart) / 1000.0f;
    float pos = cos((t + PI / 2 + .2f) * 5) * .5f + .5f;

    if (strip->invertStrip)
        pos = 1 - pos;

    float radius = .3 - (cos(pos * PI * 2) * .5f + .5f) * .25f;
    float alpha = 1;

    if (connectionState != WifiComponent::Connecting)
    {
        Color targetColor;

        switch (connectionState)
        {
        case WifiComponent::ConnectionState::Connected:
            targetColor = Color(50, 255, 0);
            break;
        case WifiComponent::ConnectionError:
            targetColor = Color(255, 55, 0);
            break;
        case WifiComponent::Disabled:
            targetColor = Color(255, 0, 255);
            break;
        case WifiComponent::Hotspot:
            targetColor = Color(255, 255, 0);
            break;

        default:
            break;
        }

        float blendFac = min(relT * 2, 1.f);
        alpha = constrain(2 * (1 - relT / animTime), 0, 1);
        color = color.lerp(targetColor, blendFac).withMultipliedAlpha(alpha);

        radius = max(radius, relT * 3 / animTime); // increase radius to 1 in one second
    }
    else
    {
        // DBG("Rel T " + String(relT));
        color = color.withMultipliedAlpha(constrain((relT - .05f) * 1 / animTime, 0, 1));
    }

    fillAll(Color(0, 0, 0, alpha * 255)); // clear strip
    point(color, pos, radius, false);
}

void LedStripSystemLayer::updateShutdown()
{
    if (!RootComponent::instance->isShuttingDown())
        return;

    float relT = (millis() - RootComponent::instance->timeAtShutdown) / 1000.0f;
    const float animTime = 1.0f;

    float t = relT / animTime;

    Color c = BatteryComponent::instance->getBatteryColor();
    c = c.withMultipliedAlpha(min(t * 2, 1.f));
    float end = constrain((1 - t) * 2, 0, 1);

    if (strip->invertStrip)
        fillRange(c, 1 - end, 1);
    else
        fillRange(c, 0, end);
}