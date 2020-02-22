#include "MainManager.h"

MainManager::MainManager(String deviceType, String fwVersion) : Component("root"),
                                                                deviceType(deviceType),
                                                                fwVersion(fwVersion),
                                                                initTimer(2000)
{
}

void MainManager::init()
{

    leds.init();

    ((EventBroadcaster<CommunicationEvent> *)&comm)->addListener(std::bind(&MainManager::communicationEvent, this, std::placeholders::_1));
    ((EventBroadcaster<ConnectionEvent> *)&comm)->addListener(std::bind(&MainManager::connectionEvent, this, std::placeholders::_1));
    comm.init();

#ifdef SLEEP_PIN
    pinMode(SLEEP_PIN, OUTPUT);
    digitalWrite(SLEEP_PIN, HIGH);
#endif

    sensors.addListener(std::bind(&MainManager::sensorEvent, this, std::placeholders::_1));
    sensors.init();

    files.addListener(std::bind(&MainManager::fileEvent, this, std::placeholders::_1));

    initTimer.addListener(std::bind(&MainManager::timerEvent, this, std::placeholders::_1));
}

void MainManager::update()
{
    initTimer.update();

    files.update();
    if (files.isUploading)
        return;

    comm.update();
    leds.update();
    sensors.update();
}

void MainManager::sleep()
{
    NDBG("Sleep now ! ");
    leds.shutdown(CRGB::Orange); //to replace with battery color

    delay(500);

#ifdef SLEEP_PIN
    digitalWrite(SLEEP_PIN, LOW);
#endif

#ifdef SLEEP_WAKEUP_BUTTON
    esp_sleep_enable_ext0_wakeup(SLEEP_WAKEUP_BUTTON, HIGH);
    esp_deep_sleep_start();
#endif
}

void MainManager::connectionEvent(const ConnectionEvent &e)
{
    NDBG("Connection Event : " + connectionStateNames[e.type] + (e.type == Connected ? "(" + comm.wifiManager.getIP() + ")" : ""));
    leds.setConnectionState(e.type);

    
    if (e.source == "wifi")
    {
        if(e.type == Connected || e.type == Hotspot){
            NDBG("Connected with IP " + comm.wifiManager.getIP());
            comm.oscManager.setEnabled(true);
        }else
        {
            comm.oscManager.setEnabled(false);
        }

        if(e.type == Connected || e.type == ConnectionError || e.type == Hotspot)
        {
            initTimer.start();
        }
    }
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
    String command = "";
    switch (e.type)
    {
    case SensorEvent::ButtonUpdate:
    {
        ButtonEvent::Type btEventType = (ButtonEvent::Type)e.data[0].intValue();
        command = ButtonEvent::eventNames[btEventType];
        switch (btEventType)
        {
        case ButtonEvent::Pressed:
            if (e.data[1].intValue() == 1)
            {
                if (comm.wifiManager.state == Connecting)
                    comm.wifiManager.disable();
            }
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

    case SensorEvent::OrientationUpdate:
    {
        IMUEvent::Type imuEventType = (IMUEvent::Type)e.data[0].intValue();
        command = IMUEvent::eventNames[imuEventType];
        switch (imuEventType)
        {
        case IMUEvent::OrientationUpdate:
        {
            //NDBG(String(event.orientation.x, 4) + " " + String(event.orientation.y, 4) + " " + String(event.orientation.z, 4));
        }
        break;
        }
    }
    break;

    case SensorEvent::BatteryUpdate:
    {
        BatteryEvent::Type batteryEventType = (BatteryEvent::Type)e.data[0].intValue();
        command = BatteryEvent::eventNames[batteryEventType];
    }
    break;
    }

    if (command.length() > 0)
        comm.sendMessage(e.source, command, e.data + 1, e.numData - 1); //data+1 and numData -1 to remove the subEventType data
}

void MainManager::fileEvent(const FileEvent &e)
{

    if (e.type == FileEvent::UploadStart)
    {
        leds.setMode(LedManager::System);
        leds.sysLedMode.uploadFeedback = true;
        comm.oscManager.setEnabled(false);
    }
    else if (e.type == FileEvent::UploadProgress)
    {
        leds.sysLedMode.showUploadProgress(e.data.floatValue());
    }
    else if (e.type == FileEvent::UploadComplete)
    {
        leds.sysLedMode.uploadFeedback = false;
        leds.setMode(LedManager::Stream);
        comm.oscManager.setEnabled(true);
    }
}

void MainManager::timerEvent(const TimerEvent &e)
{
    NDBG("Timer Event, init fileSystem and IMU");
    files.init();
    sensors.imuManager.init();
}

bool MainManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "sleep", numData, 0))
    {
        sleep();
        return true;
    }
    else if (checkCommand(command, "restart", numData, 0))
    {
        ESP.restart();
        return true;
    }

    return false;
}
