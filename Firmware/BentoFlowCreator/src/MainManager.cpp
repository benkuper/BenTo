#include "MainManager.h"

MainManager::MainManager(String deviceType, String fwVersion) :
    Component("root"),
    deviceType(deviceType),
    fwVersion(fwVersion)
{
    
}

void MainManager::init() 
{
    comm.init();
    comm.addListener(std::bind(&MainManager::communicationEvent, this, std::placeholders::_1));

    leds.init();
    sensors.init();
    files.init();
}

void MainManager::update() 
{
    comm.update();
    leds.update();
    sensors.update();
    files.update();
}

void MainManager::communicationEvent(const CommunicationEvent &e)
{
    NDBG(e.toString());
}
