#pragma once

#define LEDSTREAM_MAX_LEDS 1000
#define LEDSTREAM_MAX_PACKET_SIZE LEDSTREAM_MAX_LEDS * 4 + 1

#define LEDSTREAM_RECEIVE_PORT 8888
#define LEDSTREAM_ARTNET_PORT 5678

class LedStripStreamLayer : public LedStripLayer
{
public:
    LedStripStreamLayer(LedStripComponent *strip) : LedStripLayer("stream", LedStripLayer::Stream, strip, Type_StreamLayer) {}
    ~LedStripStreamLayer() {}

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;
};

class LedStreamReceiverComponent : public Component
{
public:
    LedStreamReceiverComponent() : Component(Type_StreamLayerReceiver) { instance = this; }
    ~LedStreamReceiverComponent() {}

    DeclareSingleton(LedStreamReceiverComponent);

    WiFiUDP udp;
    bool udpIsInit;
    Parameter *receiveRate;
    ArtnetWifi artnet;

    Parameter *useArtnet;
    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    void receiveUDP();
    void onEnabledChanged() override;

    void setupConnection();

    void onParameterEventInternal(const ParameterEvent &e) override;

    long lastReceiveTime;
    uint8_t streamBuffer[LEDSTREAM_MAX_PACKET_SIZE];
    int byteIndex;

    std::vector<LedStripStreamLayer *> layers;

    void registerLayer(LedStripStreamLayer *layer);
    void unregisterLayer(LedStripStreamLayer *layer);
    static void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data);
};