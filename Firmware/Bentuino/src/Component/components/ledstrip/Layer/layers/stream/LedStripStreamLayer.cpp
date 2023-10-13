ImplementSingleton(LedStreamReceiverComponent);

bool LedStripStreamLayer::initInternal(JsonObject o)
{
    LedStripLayer::initInternal(o);

    AddIntParam(universe);
    AddBoolParam(clearOnNoReception);
    AddFloatParam(noReceptionTime);

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

    AddBoolParam(useArtnet);
    artnet.setArtDmxCallback(&LedStreamReceiverComponent::onDmxFrame);

    setupConnection();
    return true;
}

void LedStreamReceiverComponent::updateInternal()
{
    if (!serverIsInit)
        return;

    long curTime = millis();
    if (curTime > lastUDPReceiveTime + (1000 / receiveRate))
    {
        lastUDPReceiveTime = curTime;
        if (useArtnet)
            artnet.read();
        else
            receiveUDP();
    }
}

void LedStreamReceiverComponent::clearInternal()
{
    udp.flush();
    udp.stop();
}

void LedStreamReceiverComponent::receiveUDP()
{
    if (!udp.available())
        return;

    while (udp.parsePacket())
    {
        //  NDBG("Packet available : " + String(size));
        byteIndex += udp.read(streamBuffer + byteIndex, LEDSTREAM_MAX_PACKET_SIZE - byteIndex);

        // NDBG("Received : " + String(byteIndex));

        bool isFinal = streamBuffer[byteIndex - 1] == 255;

        if (isFinal)
        {

            int stripIndex = streamBuffer[0];

            // NDBG("Is Final : " + String(stripIndex));

            if (stripIndex < layers.size())
            {
                LedStripStreamLayer *layer = layers[stripIndex];

                memcpy((uint8_t *)layer->colors, streamBuffer + 1, byteIndex - 2);
            }
            else
            {
                NDBG("Strip " + String(stripIndex) + " does not exist");
            }

            byteIndex = 0;
        }

        if (byteIndex >= LEDSTREAM_MAX_PACKET_SIZE)
            break;
    }
}

void LedStreamReceiverComponent::onEnabledChanged()
{
    setupConnection();
}

void LedStreamReceiverComponent::setupConnection()
{
    bool shouldConnect = enabled && WifiComponent::instance->state == WifiComponent::Connected;

    serverIsInit = false;

    if (shouldConnect)
    {

        if (useArtnet)
        {
            NDBG("Start Receive Led Stream on Artnet");
            artnet.begin();
            NDBG("Artnet started");
        }
        else
        {
            NDBG("Start Receive Led Stream on UDP " + String(LEDSTREAM_RECEIVE_PORT));
            udp.begin(LEDSTREAM_RECEIVE_PORT);
            udp.flush();
        }

        serverIsInit = true;
    }
    else
    {
        NDBG("Stop receiving");
        udp.flush();
        udp.stop();
        // artnet.stop();
    }
}

void LedStreamReceiverComponent::paramValueChangedInternal(void *param)
{
    if (param == &useArtnet)
    {
        if (enabled)
        {
            NDBG("Start Receive Led Stream on UDP " + String(LEDSTREAM_RECEIVE_PORT));

            if (useArtnet)
            {
                udp.flush();
                udp.stop();
                artnet.begin();
            }
            else
            {
                // artnet.stop();
                udp.begin(8888);
                udp.flush();
            }
        }
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
    for (auto &layer : instance->layers)
    {
        if (universe != layer->universe)
            continue;
        // DBG("Received Artnet " + String(universe) + " " + String(length) + " " + String(sequence) + " " + String(stripIndex) + " " + String(layer->strip->count));
        for (int i = 0; i < layer->strip->count && i < length; i++)
            layer->colors[i] = Color(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);

        layer->lastReceiveTime = millis() / 1000.0f;
        layer->hasCleared = false;
        // memcpy((uint8_t *)layer->colors, streamBuffer + 1, byteIndex - 2);
    }
}