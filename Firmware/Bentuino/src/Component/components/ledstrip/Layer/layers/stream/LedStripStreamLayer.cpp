ImplementSingleton(LedStreamReceiverComponent);

bool LedStripStreamLayer::initInternal()
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
    LedStreamReceiverComponent::instance->unregisterLayer(this);
}

bool LedStreamReceiverComponent::initInternal()
{
    receiveRate = addParameter("receiveRate", 50, 1, 200);
    byteIndex = 0;

    return true;
}

void LedStreamReceiverComponent::updateInternal()
{

    if (!enabled->boolValue())
        return;

    long curTime = millis();

    if (curTime > lastReceiveTime + (1000 / receiveRate->intValue()))
    {
        lastReceiveTime = curTime;
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
        udp.begin(8888);
        udp.flush();
    }
    else
    {
        NDBG("Stop receiving UDP Led Stream");
        udp.flush();
        udp.stop();
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