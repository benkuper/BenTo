#pragma once

#include <Arduino.h>

#include "communication/CommunicationManager.h"
#include "leds/LedManager.h"
#include <FastLED.h>

#include "files/FileManager.h"

#ifdef HAS_BATTERY
#include "sensors/battery/BatteryManager.h"
#endif

#if BUTTON_COUNT
#include "sensors/button/ButtonManager.h"
#endif

#ifdef HAS_TOUCH
#include "sensors/touch/TouchManager.h"
#endif

#ifdef HAS_IMU
#include "sensors/imu/IMUManager.h"
#endif

#ifdef HAS_CAPA
#include "sensors/capacitive/CapacitiveManager.h"
#endif

#ifdef HAS_MIC
#include "sensors/mic/MicManager.h"
#endif

#ifdef HAS_PWM
#include "pwm/PWMManager.h"
#endif

#ifdef HAS_SCRIPTS
#include "scripts/ScriptManager.h"
#endif

class MainManager : public Component
{
public:
    MainManager(String fwVersion);
    ~MainManager() {}

    static MainManager *instance;

    long tstart;
    long tend;

    String hardwareID;
    String fwVersion;

#ifdef HAS_DISPLAY
    DisplayManager display;
#endif

    CommunicationManager comm;
#ifdef LED_COUNT
    LedManager leds;
#endif

#ifdef HAS_PWM
    PWMManager pwm;
#endif

#ifdef HAS_FILES
    FileManager files;
#endif

#ifdef HAS_BATTERY
    BatteryManager battery;
#endif

#if BUTTON_COUNT
    ButtonManager buttons;
#endif

#ifdef HAS_TOUCH
    TouchManager touch;
#endif

#ifdef HAS_IMU
    IMUManager imu;
#endif

#ifdef HAS_CAPA
    CapacitiveManager cap;
#endif

#ifdef HAS_MIC
    MicManager mic;
#endif

#ifdef HAS_SCRIPTS
    ScriptManager scripts;
#endif

    Timer initTimer; // to init blocking stuff after connection

    void init();
    void update();

    void sleep(CRGB color = CRGB::Cyan);

    void connectionEvent(const ConnectionEvent &e);
    void communicationEvent(const CommunicationEvent &e);

#ifdef HAS_BATTERY
    void batteryEvent(const BatteryEvent &e);
#endif

#if BUTTON_COUNT
    void buttonEvent(const ButtonEvent &e);
#endif

#ifdef HAS_TOUCH
    void touchEvent(const TouchEvent &e);
#endif

#ifdef HAS_IMU
    void imuEvent(const IMUEvent &e);
#endif

#ifdef HAS_CAPA
    void capacitiveEvent(const CapacitiveEvent &e);
#endif

#ifdef HAS_MIC
    void micEvent(const MicEvent &e);
#endif

#ifdef LED_COUNT
    void rgbLedsEvent(const RGBLedsEvent &e);
#endif

#if HAS_FILES
    void fileEvent(const FileEvent &e);
#endif

    void timerEvent(const TimerEvent &e);

    bool handleCommand(String command, var *data, int numData) override;
};