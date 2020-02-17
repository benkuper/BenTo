#include "SensorManager.h"

SensorManager::SensorManager() : Component("sensors")
{
}

void SensorManager::init()
{
    btManager.init();
    btManager.addListener(std::bind(&SensorManager::buttonEvent, this, std::placeholders::_1));
}

void SensorManager::update()
{
    btManager.update();
}

void SensorManager::buttonEvent(const ButtonEvent &e)
{

    int numBTData = e.type == ButtonEvent::MultiPress ? 1 : 0;
    var *data = (var *)malloc((numBTData + 1) * sizeof(var));
    data[0].value.i = e.type;
    data[0].type = 'i';

    if (e.type == ButtonEvent::MultiPress)
    {
        data[1].value.i = e.count;
        data[1].type = 'i';
    }
    
    sendEvent(SensorEvent(SensorEvent::ButtonUpdate, data, numBTData + 1));
}
