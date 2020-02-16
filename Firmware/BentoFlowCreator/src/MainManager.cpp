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

    
void MainManager::sleep() 
{
    esp_sleep_enable_ext0_wakeup(SLEEP_WAKEUP_BUTTON, HIGH);
    esp_deep_sleep_start();
}


void MainManager::communicationEvent(const CommunicationEvent &e)
{
    NDBG(e.toString());
    Component  * c = Component::getComponentForName(e.target);

    if(c != nullptr) 
    {
        bool result = c->handleCommand(e.command, e.data, e.numData);
        if(!result) NDBG("Command not handled : "+e.target+"."+e.command);
    }
    else 
    {
        NDBG("Target not found : "+e.target);
    }
}


bool MainManager::handleCommand(String command, var * data, int numData)
{
    if(checkCommand(command, "sleep", numData, 0))
    {
        sleep();
        return true;
    }
    
    return false;
}