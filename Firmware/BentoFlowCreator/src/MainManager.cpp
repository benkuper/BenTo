#include "MainManager.h"

MainManager::MainManager(String deviceType, String fwVersion) : Component("root"),
                                                                deviceType(deviceType),
                                                                fwVersion(fwVersion)
{
}

void MainManager::init()
{
    leds.init();
    
    ((EventBroadcaster<CommunicationEvent> *)&comm)->addListener(std::bind(&MainManager::communicationEvent, this, std::placeholders::_1));
    ((EventBroadcaster<ConnectionEvent> *)&comm)->addListener(std::bind(&MainManager::connectionEvent, this, std::placeholders::_1));
    comm.init();

    sensors.addListener(std::bind(&MainManager::sensorEvent, this, std::placeholders::_1));
    sensors.init();
    
    files.addListener(std::bind(&MainManager::fileEvent, this, std::placeholders::_1));
    files.init();
}

void MainManager::update()
{
    files.update();
    if(files.isUploading) return;
    
    comm.update();
    leds.update();
    sensors.update();
}

void MainManager::sleep()
{
    //NDBG("Sleep now ! ");
    leds.shutdown(CRGB::Orange); //to replace with battery color

    delay(500);

    esp_sleep_enable_ext0_wakeup(SLEEP_WAKEUP_BUTTON, HIGH);
    esp_deep_sleep_start();
}

void MainManager::connectionEvent(const ConnectionEvent &e)
{
    NDBG("Connection Event : " + connectionStateNames[e.type] + (e.type == Connected ? "(" + comm.wifiManager.getIP() + ")" : ""));
    leds.setConnectionState(e.type);
   
    if(e.source == "wifi") 
    {
        switch(e.type)
        {
            case Connected :
                comm.oscManager.setEnabled(true);
                files.initServer();
                break;

            case Off:
            case ConnectionError:
            case Disabled:
                comm.oscManager.setEnabled(false);
                files.closeServer();
                break;
        }
    }

}

void MainManager::communicationEvent(const CommunicationEvent &e)
{
    //NDBG(e.toString());

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
            if(comm.wifiManager.state == Connecting) comm.wifiManager.disable();
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

void MainManager::fileEvent(const FileEvent &e) {
    
    if(e.type == FileEvent::UploadStart) 
    {
        leds.setMode(LedManager::System);
        leds.sysLedMode.uploadFeedback = true;
        comm.oscManager.setEnabled(false);
    }
    else if(e.type == FileEvent::UploadProgress)
    {
        leds.sysLedMode.showUploadProgress(e.data.floatValue());
    }else if(e.type == FileEvent::UploadComplete)
    {
        leds.sysLedMode.uploadFeedback = false;
        leds.setMode(LedManager::Stream);
        comm.oscManager.setEnabled(true);
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
