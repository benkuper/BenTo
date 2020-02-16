#pragma once

#include <Arduino.h>

#include "communication/CommunicationManager.h"
#include "leds/LedManager.h"
#include "sensors/SensorManager.h"
#include "files/FileManager.h"

class MainManager :
    public Component
{
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

    void init();
    void update();

    void communicationEvent(const CommunicationEvent &e);
};