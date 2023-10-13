#pragma once

#define LEDSTREAM_MAX_LEDS 1000
#define LEDSTREAM_MAX_PACKET_SIZE LEDSTREAM_MAX_LEDS * 4 + 1

#define LEDSTREAM_RECEIVE_PORT 8888
#define LEDSTREAM_ARTNET_PORT 5678

class LedStripStreamLayer : public LedStripLayer
{
public:
    LedStripStreamLayer(LedStripComponent *strip) : LedStripLayer("streamLayer", LedStripLayer::Stream, strip) {}
    ~LedStripStreamLayer() {}

    DeclareIntParam(universe, 0);
    DeclareBoolParam(clearOnNoReception, false);
    DeclareFloatParam(noReceptionTime, 1.0f);

    bool hasCleared = false;
    float lastReceiveTime = 0;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    HandleSetParamInternalStart
    HandleSetParamInternalMotherClass(LedStripLayer)
        CheckAndSetParam(universe);
    CheckAndSetParam(clearOnNoReception);
    CheckAndSetParam(noReceptionTime);
    HandleSetParamInternalEnd;

    FillSettingsInternalStart
    FillSettingsInternalMotherClass(LedStripLayer)
        FillSettingsParam(universe);
    FillSettingsParam(clearOnNoReception);
    FillSettingsParam(noReceptionTime);
    FillSettingsInternalEnd;

    FillOSCQueryInternalStart
    FillOSCQueryInternalMotherClass(LedStripLayer)
        FillOSCQueryIntParam(universe);
    FillOSCQueryBoolParam(clearOnNoReception);
    FillOSCQueryFloatParam(noReceptionTime);
    FillOSCQueryInternalEnd;
};

// Stream receiver

DeclareComponentSingleton(LedStreamReceiver, "streamReceiver", )

    WiFiUDP udp;
bool serverIsInit;
float lastUDPReceiveTime = 0;

DeclareIntParam(receiveRate, 60);
DeclareBoolParam(useArtnet, true);

ArtnetWifi artnet;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void receiveUDP();
void onEnabledChanged() override;

void setupConnection();

void paramValueChangedInternal(void *param) override;

uint8_t streamBuffer[LEDSTREAM_MAX_PACKET_SIZE];
int byteIndex;

std::vector<LedStripStreamLayer *> layers;

void registerLayer(LedStripStreamLayer *layer);
void unregisterLayer(LedStripStreamLayer *layer);
static void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data);

HandleSetParamInternalStart
    CheckAndSetParam(receiveRate);
CheckAndSetParam(useArtnet);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(receiveRate);
FillSettingsParam(useArtnet);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryIntParam(receiveRate);
FillOSCQueryBoolParam(useArtnet);
FillOSCQueryInternalEnd

    EndDeclareComponent