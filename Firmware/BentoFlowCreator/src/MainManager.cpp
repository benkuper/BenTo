#include "MainManager.h"

MainManager::MainManager(String deviceType, String fwVersion) :
    deviceType(deviceType),
    fwVersion(fwVersion)
{
    
}

void MainManager::init() 
{
    comm.init();
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
