#pragma once

#include <Arduino.h>

#include "communication/CommunicationManager.h"
#include "leds/LedManager.h"
#include "files/FileManager.h"
#include "sensors/battery/BatteryManager.h"
#include "sensors/button/ButtonManager.h"
#include "sensors/touch/TouchManager.h"
#include "sensors/imu/IMUManager.h"
#include "sensors/capacitive/CapacitiveManager.h"
#include "sensors/mic/MicManager.h"
#include "pwm/PWMManager.h"
#include "scripts/ScriptManager.h"

class MainManager : public Component {
public:
    MainManager(String fwVersion);
    ~MainManager() {}

    static MainManager * instance;
    
    long tstart;
    long tend;

    String hardwareID;
    String fwVersion;

#ifdef HAS_DISPLAY
    DisplayManager display;
#endif

    CommunicationManager comm;
    LedManager leds;
    PWMManager pwm;
    FileManager files;

    BatteryManager battery;
    ButtonManager buttons;
    TouchManager touch;
    IMUManager imu;
    CapacitiveManager cap;
    MicManager mic;
    ScriptManager scripts;

    Timer initTimer; //to init blocking stuff after connection

    void init();
    void update();

    void sleep(CRGB color = CRGB::Cyan);

    void connectionEvent(const ConnectionEvent &e);
    void communicationEvent(const CommunicationEvent &e);
    
    void batteryEvent(const BatteryEvent &e);
    void buttonEvent(const ButtonEvent &e);
    void touchEvent(const TouchEvent &e);
    void imuEvent(const IMUEvent &e);
    void capacitiveEvent(const CapacitiveEvent &e);
    void micEvent(const MicEvent &e);
    void rgbLedsEvent(const RGBLedsEvent &e);

    void fileEvent(const FileEvent &e);
    void timerEvent(const TimerEvent &e);

    bool handleCommand(String command, var *data, int numData) override;
 };