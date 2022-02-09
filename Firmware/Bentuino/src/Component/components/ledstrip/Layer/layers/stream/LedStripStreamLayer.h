#pragma once

#define LEDSTREAM_MAX_LEDS 1000
#define LEDSTREAM_MAX_PACKET_SIZE LEDSTREAM_MAX_LEDS * 4 + 1

#define LEDSTREAM_RECEIVE_PORT 8888

class LedStripStreamLayer : public LedStripLayer
{
public:
    LedStripStreamLayer(const String &name, LedStripComponent *strip) : LedStripLayer(name, LedStripLayer::Stream, strip) {}
    ~LedStripStreamLayer() {}

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;
};

DeclareComponentSingleton(LedStreamReceiver, "streamReceiver", )

    WiFiUDP udp;
Parameter *receiveRate;

bool initInternal(JsonObject o) override;
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