#pragma once
#include "../LedMode.h"

#ifdef LED_COUNT

class SystemLedMode : public LedMode {
public:
    SystemLedMode(CRGB * leds, int numLeds);
    ~SystemLedMode() {}
    
    ConnectionState connectionState;
    float timeAtStateChange;
    float timeAtConnecting;
    const long stateAnimationTime = 1000;

    float pointPos;

    bool uploadFeedback;
    float uploadProgress;

    void init() override;
    void update() override;

    void setBatteryColor(CRGB color);
    void setConnectionState(ConnectionState state);
    void showUploadProgress(float value);
};

#endif //LED_COUNT