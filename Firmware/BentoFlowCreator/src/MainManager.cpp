#include "MainManager.h"

MainManager::MainManager(String deviceType, String fwVersion) : Component("root"),
                                                                deviceType(deviceType),
                                                                fwVersion(fwVersion)
{
}

void MainManager::init()
{
    leds.init();
    //leds.sysLedMode.setBattery(sensors.battery.getVoltage());

    ((EventBroadcaster<CommunicationEvent> *)&comm)->addListener(std::bind(&MainManager::communicationEvent, this, std::placeholders::_1));
    ((EventBroadcaster<ConnectionEvent> *)&comm)->addListener(std::bind(&MainManager::connectionEvent, this, std::placeholders::_1));
    comm.init();

    sensors.addListener(std::bind(&MainManager::sensorEvent, this, std::placeholders::_1));
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
    NDBG("Sleep now ! ");
    leds.shutdown(CRGB::Orange); //to replace with battery color

    delay(500);

    esp_sleep_enable_ext0_wakeup(SLEEP_WAKEUP_BUTTON, HIGH);
    esp_deep_sleep_start();
}

void MainManager::connectionEvent(const ConnectionEvent &e)
{
    NDBG("Connection Event : " + connectionStateNames[e.type] + (e.type == Connected ? "(" + comm.wifiManager.getIP() + ")" : ""));
    leds.sysLedMode.setConnectionState(e.type);
}

void MainManager::communicationEvent(const CommunicationEvent &e)
{
    NDBG(e.toString());

    Component *c = Component::getComponentForName(e.target);

    if (c != nullptr)
    {
        bool result = c->handleCommand(e.command, e.data, e.numData);
        if (!result)
            NDBG("Command not handled : " + e.target + "." + e.command);
    }
    else
    {
        NDBG("Target not found : " + e.target);
    }
}

void MainManager::sensorEvent(const SensorEvent &e)
{
    switch (e.type)
    {
    case SensorEvent::ButtonUpdate:
    {
        ButtonEvent::Type btEventType = (ButtonEvent::Type)e.data[0].intValue();
        switch (btEventType)
        {
        case ButtonEvent::Pressed:
            break;

        case ButtonEvent::Released:
            break;

        case ButtonEvent::ShortPress:
            break;

        case ButtonEvent::LongPress:
            break;

        case ButtonEvent::VeryLongPress:
        {
            sleep();
        }
        break;

        case ButtonEvent::MultiPress:
        {
            int count = e.data[1].intValue();
        }
        break;
        }
    }
    break;
    }
}

bool MainManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "sleep", numData, 0))
    {
        sleep();
        return true;
    }

    return false;
}