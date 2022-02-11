ImplementSingleton(LedStreamReceiverComponent);

bool LedStripStreamLayer::initInternal(JsonObject o)
{
    LedStreamReceiverComponent::instance->registerLayer(this);

    return true;
}

void LedStripStreamLayer::updateInternal()
{
    if (!enabled->boolValue())
        return;
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
    receiveRate = addParameter("receiveRate", 50, 1, 200);
    byteIndex = 0;

    useArtnet = addParameter("useArtnet", false);

    artnet.setArtDmxCallback(&LedStreamReceiverComponent::onDmxFrame);
    return true;
}

void LedStreamReceiverComponent::updateInternal()
{

    if (!enabled->boolValue())
        return;

    if (useArtnet->boolValue())
    {
        artnet.read();
    }
    else
    {
        long curTime = millis();

        if (curTime > lastReceiveTime + (1000 / receiveRate->intValue()))
        {
            lastReceiveTime = curTime;
            receiveUDP();
        }
    }
}

void LedStreamReceiverComponent::clearInternal()
{
    udp.flush();
    udp.stop();
}

void LedStreamReceiverComponent::receiveUDP()
{
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
    if (enabled->boolValue())
    {
        NDBG("Start Receive Led Stream on UDP " + String(LEDSTREAM_RECEIVE_PORT));

        if (useArtnet->boolValue())
        {
            artnet.begin();
        }
        else
        {
            udp.begin(8888);
            udp.flush();
        }
    }
    else
    {
        NDBG("Stop receiving UDP Led Stream");
        udp.flush();
        udp.stop();
        // artnet.stop();
    }
}

void LedStreamReceiverComponent::onParameterEventInternal(const ParameterEvent &e)
{
    if (e.parameter == useArtnet)
    {
        if (enabled->boolValue())
        {
            NDBG("Start Receive Led Stream on UDP " + String(LEDSTREAM_RECEIVE_PORT));

            if (useArtnet->boolValue())
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
    int stripIndex = universe;

    if (stripIndex < instance->layers.size())
    {
        LedStripStreamLayer *layer = instance->layers[stripIndex];

        for (int i = 0; i < layer->numLeds && i < length; i++) layer->colors[i] = Color(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
        // memcpy((uint8_t *)layer->colors, streamBuffer + 1, byteIndex - 2);
    }
    else
    {
        DBG("Strip " + String(stripIndex) + " does not exist");
    }
}