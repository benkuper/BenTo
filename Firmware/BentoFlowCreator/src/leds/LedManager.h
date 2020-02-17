#pragma once

#include "system/SystemLedMode.h"
#include "output/RGBLedsManager.h"

class LedManager : public Component {
public:
    LedManager();
    ~LedManager(){}
    
    enum Mode { Direct, System, Stream, Player };
    Mode mode;

    //source
    LedMode * currentMode;
    SystemLedMode sysLedMode;

    //out
    RGBLedsManager rgbManager;
    
    void init();
    void update();

    void setMode(Mode m);

    void wakeup(CRGB color);
    void shutdown(CRGB color);

    bool handleCommand(String command, var *data, int numData) override;
};