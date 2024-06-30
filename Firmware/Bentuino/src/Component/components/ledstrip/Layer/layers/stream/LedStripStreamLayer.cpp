ImplementSingleton(LedStreamReceiverComponent);

bool LedStripStreamLayer::initInternal(JsonObject o)
{
    LedStripLayer::initInternal(o);

    AddIntParamConfig(universe);
    AddBoolParamConfig(clearOnNoReception);
    AddFloatParamConfig(noReceptionTime);

    LedStreamReceiverComponent::instance->registerLayer(this);

    return true;
}

void LedStripStreamLayer::updateInternal()
{
    if (!hasCleared && clearOnNoReception && millis() / 1000.0f - lastReceiveTime > noReceptionTime)
    {
        clearColors();
        hasCleared = true;
    }
}

void LedStripStreamLayer::clearInternal()
{
    if (LedStreamReceiverComponent::instance != nullptr)
    {
        LedStreamReceiverComponent::instance->unregisterLayer(this);
    }
}

bool LedStreamReceiverComponent::initInternal(JsonObject o)
{
    serverIsInit = false;
    AddIntParam(receiveRate);
    byteIndex = 0;

    artnet.setArtDmxCallback(&LedStreamReceiverComponent::onDmxFrame);

    setupConnection();
    return true;
}

void LedStreamReceiverComponent::updateInternal()
{
    if (!serverIsInit)
        return;

    long curTime = millis();
    if (curTime > lastReceiveTime + (1000 / max(receiveRate, 1)))
    {
        lastReceiveTime = curTime;
        int r = -1;
        while (r != 0)
        {
            r = artnet.read();
        }
    }
}

void LedStreamReceiverComponent::clearInternal()
{
    // artnet.stop(); //when it will be implemented
}

void LedStreamReceiverComponent::onEnabledChanged()
{
    setupConnection();
}

void LedStreamReceiverComponent::setupConnection()
{
    bool shouldConnect = enabled && WifiComponent::instance->state == WifiComponent::Connected;

    if (shouldConnect)
    {
        NDBG("Start Receive Led Stream on Artnet");
        artnet.begin();
        NDBG("Artnet started");
        serverIsInit = true;
    }
    else
    {
        artnet.stop();
        serverIsInit = false;
    }
}

void LedStreamReceiverComponent::registerLayer(LedStripStreamLayer *layer)
{
    layers.push_back(layer);
}

void LedStreamReceiverComponent::unregisterLayer(LedStripStreamLayer *layer)
{
    for (int i = 0; i < layers.size(); i++)
    {
        if (layers[i] == layer)
        {
            layers.erase(layers.begin() + i);
            return;
        }
    }
}

void LedStreamReceiverComponent::onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
{
    // DBG("Received Artnet "+String(universe));

    float multiplier = 1.0f;
    if (RootComponent::instance->isShuttingDown())
    {
        float relT = (millis() - RootComponent::instance->timeAtShutdown) / 1000.0f;
        const float animTime = 1.0f;
        multiplier = max(1 - relT * 2 / animTime, 0.f);
    }

    for (auto &layer : instance->layers)
    {
        int numUniverses = std::ceil(layer->strip->count * 1.0f / 170); // 170 leds per universe
        if (universe < layer->universe || universe > layer->universe + numUniverses - 1)
            continue;

        // DBG("Received Artnet " + String(universe) + " " + String(length) + " " + String(sequence) + " " + String(stripIndex) + " " + String(layer->strip->count));

        int start = (universe - layer->universe) * 170;

        // DBG("Received Artnet " + String(universe) + ", start = " + String(start));
        for (int i = 0; i < layer->strip->count && i < 170 && (i * 3) < length; i++)
        {
            layer->colors[i + start] = Color(data[i * 3] * multiplier, data[i * 3 + 1] * multiplier, data[i * 3 + 2] * multiplier);
        }

        layer->lastReceiveTime = millis() / 1000.0f;
        layer->hasCleared = false;
        // memcpy((uint8_t *)layer->colors, streamBuffer + 1, byteIndex - 2);
    }
}