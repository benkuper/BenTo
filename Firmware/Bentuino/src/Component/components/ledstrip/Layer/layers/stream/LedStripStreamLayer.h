#pragma once

#define LEDSTREAM_MAX_LEDS 1000
#define LEDSTREAM_LED_CHANNELS 4 // ARGB
#define LEDSTREAM_MAX_PACKET_SIZE LEDSTREAM_MAX_LEDS *LEDSTREAM_LED_CHANNELS + 1

#define LEDSTREAM_RECEIVE_PORT 8888

class LedStripStreamLayer : public LedStripLayer
{
public:
    LedStripStreamLayer(LedStripComponent *strip) : LedStripLayer("stream", LedStripLayer::STREAM, strip) {}
    ~LedStripStreamLayer() {}

    bool initInternal() override;
    void updateInternal() override;
    void clearInternal() override;
};


DeclareComponentSingleton(LedStreamReceiver, "streamReceiver", )

    WiFiUDP udp;
Parameter *receiveRate;

bool initInternal() override;
void updateInternal() override;
void clearInternal() override;

void receiveUDP();
void onEnabledChanged() override;

long lastReceiveTime;
uint8_t streamBuffer[LEDSTREAM_MAX_PACKET_SIZE];
int byteIndex;

std::vector<LedStripStreamLayer *> layers;

void registerLayer(LedStripStreamLayer *layer);
void unregisterLayer(LedStripStreamLayer *layer);

EndDeclareComponent