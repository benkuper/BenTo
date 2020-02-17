#pragma once
#include "../LedMode.h"

class SystemLedMode : public LedMode {
public:
    SystemLedMode(CRGB * leds, int numLeds);
    ~SystemLedMode() {}
    
    ConnectionState connectionState;
    float timeAtStateChange;
    float timeAtConnecting;
    const long stateAnimationTime = 1000;

    float pointPos;

    void init() override;
    void update() override;

    void setBatteryColor(CRGB color);
    void setConnectionState(ConnectionState state);

    bool handleCommand(String command, var *data, int numData) override;
};