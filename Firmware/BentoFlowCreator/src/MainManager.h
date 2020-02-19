#pragma once

#include <Arduino.h>

#include "communication/CommunicationManager.h"
#include "leds/LedManager.h"
#include "sensors/SensorManager.h"
#include "files/FileManager.h"


class MainManager : public Component {
public:
    MainManager(String deviceType, String fwVersion);
    ~MainManager() {}

    String hardwareID;
    String deviceType;
    String fwVersion;

    CommunicationManager comm;
    LedManager leds;
    SensorManager sensors;
    FileManager files;

    Timer initTimer; //to init blocking stuff after connection

    void init();
    void update();

    void sleep();

    void connectionEvent(const ConnectionEvent &e);
    void communicationEvent(const CommunicationEvent &e);
    void sensorEvent(const SensorEvent &e);
    void fileEvent(const FileEvent &e);
    void timerEvent(const TimerEvent &e);

    bool handleCommand(String command, var *data, int numData) override;

 };