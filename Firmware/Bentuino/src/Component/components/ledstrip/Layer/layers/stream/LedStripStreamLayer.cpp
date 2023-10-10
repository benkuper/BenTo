ImplementSingleton(LedStreamReceiverComponent);

bool LedStripStreamLayer::initInternal(JsonObject o)
{
    LedStripLayer::initInternal(o);

    // delay(100);
    LedStreamReceiverComponent::instance->registerLayer(this);

    return true;
}

void LedStripStreamLayer::updateInternal()
{
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
    AddAndSetParameter(receiveRate);
    byteIndex = 0;

    AddAndSetParameter(useArtnet);
    artnet.setArtDmxCallback(&LedStreamReceiverComponent::onDmxFrame);
    return true;
}

void LedStreamReceiverComponent::updateInternal()
{
    if(!serverIsInit) return;

    if (useArtnet.boolValue())
    {
        // DBG("Artnet read");
        artnet.read();
    }
    else
    {
        // DBG("UDP Read");
        long curTime = millis();

        if (curTime > lastReceiveTime + (1000 / receiveRate.intValue()))
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
    if(!udp.available()) return;

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
    bool shouldConnect = enabled.boolValue() && WifiComponent::instance->state == WifiComponent::Connected;

    serverIsInit = false;

    if (shouldConnect)
    {

        if (useArtnet.boolValue())
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

void LedStreamReceiverComponent::onParameterEventInternal(const ParameterEvent &e)
{
    if (e.parameter == &useArtnet)
    {
        if (enabled.boolValue())
        {
            NDBG("Start Receive Led Stream on UDP " + String(LEDSTREAM_RECEIVE_PORT));

            if (useArtnet.boolValue())
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
    int stripIndex = universe - 1;

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