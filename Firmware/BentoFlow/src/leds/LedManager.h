#pragma once

#include "common/Common.h"

#ifdef LED_COUNT

#include "system/SystemLedMode.h"
#include "stream/StreamMode.h"
#include "player/PlayerMode.h"

#include "fx/FXManager.h"

#include "output/RGBLedsManager.h"
#include "output/IRLedsManager.h"

class LedManager : public Component {
public:
    LedManager();
    ~LedManager(){}
    
    enum Mode { Direct, System, Stream, Player };
    Mode mode;

    //source
    LedMode * currentMode;
    SystemLedMode sysLedMode;
    StreamMode streamMode;
    PlayerMode playerMode;
    //out
    RGBLedsManager rgbManager;

#if HAS_LED_FX
    FXManager fxManager;
#endif

#if HAS_IR
    IRLedsManager irManager;
#endif

    //timers
    Timer connectedTimer;

    //ping feedback
    bool connectionIsAlive;
    
    void init();
    void update();

    void setMode(Mode m);

    void shutdown(CRGB color);

    void setConnectionState(ConnectionState state);

    void rgbLedsEvent(const RGBLedsEvent &e);
    void playerEvent(const PlayerEvent &e);

    bool handleCommand(String command, var *data, int numData) override;
    void timerEvent(const TimerEvent &e);
    void showBaseOnOutLayer();
};

#endif