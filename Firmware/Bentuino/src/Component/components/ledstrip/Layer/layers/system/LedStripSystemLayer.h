#pragma once

class LedStripSystemLayer : public LedStripLayer
{
public:
    LedStripSystemLayer(LedStripComponent *strip) : LedStripLayer("system", LedStripLayer::SYSTEM, strip), systemMode(Idle), connectionMode(Connecting) {}
    ~LedStripSystemLayer() {}

    enum SystemMode { Idle, Connection, ConnectionLost };
    enum SystemConnectionMode { Connecting, Connected, ConnectionError, HotSpot };

    SystemMode systemMode;
    SystemConnectionMode connectionMode;
    unsigned long timeAtModeChanged;
    unsigned long timeAtConnectionModeChanged;

    bool initInternal() override;
    void updateInternal() override;
    void clearInternal() override;

    void setSystemMode(SystemMode m)
    {
        if(systemMode == m) return;
        systemMode = m;
        timeAtModeChanged = millis();
    }

    void setConnectionMode(SystemConnectionMode m)
    {
        if(connectionMode == m) return;
        connectionMode = m;
        timeAtConnectionModeChanged = millis();
    }
};