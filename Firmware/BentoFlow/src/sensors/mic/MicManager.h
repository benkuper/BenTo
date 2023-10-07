#pragma once
#include "../../common/Common.h"

#ifdef HAS_MIC

#include <M5StickCPlus.h>
#include <driver/i2s.h>
#include <arduinoFFT.h>

#define PIN_CLK 0
#define PIN_DATA 34
#define READ_LEN (2 * 256)
#define GAIN_FACTOR 3

#define RMS_WINDOW 10

class MicEvent
{
public:
    enum Type
    {
        Enveloppe,
        TYPES_MAX
    };

    static const String eventNames[TYPES_MAX];

    MicEvent(Type type, int id, int value = 1) : type(type), id(id), value(value) {}

    Type type;
    int id;
    int value;
};

class MicManager : public Component,
                   public EventBroadcaster<MicEvent>
{
public:
    MicManager();
    ~MicManager() {}

    static MicManager *instance;

    bool isEnabled;
    bool shouldStopRead;
    float enveloppe;

    uint8_t BUFFER[READ_LEN] = {0};
    uint16_t oldy[160];
    int16_t *adcBuffer = NULL;

    float envWindow[RMS_WINDOW]{0};
    int envIndex = 0;

    void init();
    void update();
    void shutdown();

    void i2sInit();
    void showSignal();
    static void mic_record_task(void *arg);

    void setEnabled(bool value);
    bool handleCommand(String command, var *data, int numData) override;
};

#endif