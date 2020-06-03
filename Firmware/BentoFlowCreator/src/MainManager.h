#pragma once

#include <Arduino.h>

#include "communication/CommunicationManager.h"
#include "leds/LedManager.h"
#include "files/FileManager.h"
#include "sensors/battery/BatteryManager.h"
#include "sensors/button/ButtonManager.h"
#include "sensors/imu/IMUManager.h"


class MainManager : public Component {
public:
    MainManager(String fwVersion);
    ~MainManager() {}

    String hardwareID;
    String fwVersion;
#ifdef HAS_DISPLAY
    DisplayManager display;
#endif

    CommunicationManager comm;
    LedManager leds;
    FileManager files;

    BatteryManager battery;
    ButtonManager buttons;
    IMUManager imu;

    Timer initTimer; //to init blocking stuff after connection

    void init();
    void update();

    void sleep(CRGB color = CRGB::Cyan);

    void connectionEvent(const ConnectionEvent &e);
    void communicationEvent(const CommunicationEvent &e);
    
    void batteryEvent(const BatteryEvent &e);
    void buttonEvent(const ButtonEvent &e);
    void imuEvent(const IMUEvent &e);

    void fileEvent(const FileEvent &e);
    void timerEvent(const TimerEvent &e);

    bool handleCommand(String command, var *data, int numData) override;
 };