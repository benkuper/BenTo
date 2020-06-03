#include "MainManager.h"

MainManager::MainManager(String fwVersion) : Component("root"),
                                             fwVersion(fwVersion),
                                             initTimer(2000)
{
}

void MainManager::init()
{
#ifdef HAS_DISPLAY
    display.init();
#endif

    leds.init();

    ((EventBroadcaster<CommunicationEvent> *)&comm)->addListener(std::bind(&MainManager::communicationEvent, this, std::placeholders::_1));
    ((EventBroadcaster<ConnectionEvent> *)&comm)->addListener(std::bind(&MainManager::connectionEvent, this, std::placeholders::_1));
    comm.init();

#ifdef SLEEP_PIN
#ifdef KEEP_SLEEP_PIN_HIGH
    pinMode(SLEEP_PIN, OUTPUT);
    digitalWrite(SLEEP_PIN, HIGH);
#endif
#endif

    battery.init();
    battery.addListener(std::bind(&MainManager::batteryEvent, this, std::placeholders::_1));

    buttons.init();
    buttons.addListener(std::bind(&MainManager::buttonEvent, this, std::placeholders::_1));

    imu.addListener(std::bind(&MainManager::imuEvent, this, std::placeholders::_1));

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

    battery.update();
    buttons.update();
    imu.update();

    NDBG("update");
    
}

void MainManager::sleep(CRGB color)
{
    //NDBG("Sleep now ! ");
    comm.sendMessage(name, "sleep", nullptr, 0);
    leds.shutdown(color); //to replace with battery color

    delay(500);

#ifdef SLEEP_PIN
    pinMode(SLEEP_PIN, OUTPUT);

#ifdef INFINITE_LOOP_ON_SLEEP
    while (true)
        digitalWrite(SLEEP_PIN, SLEEP_PIN_SLEEP_VAL);
#else
    digitalWrite(SLEEP_PIN, SLEEP_PIN_SLEEP_VAL);
#endif //INFINITE LOOP

#endif //SLEEP_PIN

#ifdef SLEEP_WAKEUP_BUTTON
    esp_sleep_enable_ext0_wakeup(SLEEP_WAKEUP_BUTTON, SLEEP_WAKEUP_STATE);
    esp_deep_sleep_start();
#endif

#ifdef ESP8266
    ESP.deepSleep(5e6);
#endif
}

void MainManager::connectionEvent(const ConnectionEvent &e)
{
    NDBG("Connection Event : " + connectionStateNames[e.type] + (e.type == Connected ? "(" + comm.wifiManager.getIP() + ")" : ""));
    leds.setConnectionState(e.type);

    if (e.source == "wifi")
    {
        if (e.type == Connected || e.type == Hotspot)
        {
            NDBG("Connected with IP " + comm.wifiManager.getIP());
            comm.oscManager.setEnabled(true);
        }
        else
        {
            comm.oscManager.setEnabled(false);
        }

        if (e.type == Connected || e.type == ConnectionError || e.type == Hotspot)
        {
            initTimer.start();
        }
    }
}

void MainManager::communicationEvent(const CommunicationEvent &e)
{
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

void MainManager::batteryEvent(const BatteryEvent &e)
{
    var data[1];
    data[0].type = 'f';
    data[0].value.f = e.value;
    comm.sendMessage(battery.name, BatteryEvent::eventNames[(int)e.type], data, 1);

    if (e.type == BatteryEvent::CriticalLevel)
    {

        sleep(CRGB::Red);
    }
}

void MainManager::buttonEvent(const ButtonEvent &e)
{
    int numBTData = (e.type == ButtonEvent::MultiPress || e.type == ButtonEvent::Pressed) ? 2 : 1;
    var *data = (var *)malloc((numBTData) * sizeof(var));

    data[0].value.i = e.id;
    data[0].type = 'i';

    if (numBTData > 1)
    {
        data[1].value.i = e.value;
        data[1].type = 'i';
    }

    comm.sendMessage(buttons.name, ButtonEvent::eventNames[(int)e.type], data, numBTData);

    switch (e.type)
    {
    case ButtonEvent::Pressed:
        if (e.value == 1)
        {
            if (comm.wifiManager.state == Connecting)
                comm.wifiManager.disable();
        }
        break;

    case ButtonEvent::VeryLongPress:
    {
        if(e.id == 0) sleep(); //only first button can sleep
    }
    break;
    }
}

void MainManager::imuEvent(const IMUEvent &e)
{
    switch (e.type)
    {
    case IMUEvent::OrientationUpdate:
    {
        var data[3];
        for (int i = 0; i < 3; i++)
        {
            data[i].type = 'f';
            data[i].value.f = imu.orientation[i];
        }

        comm.sendMessage(imu.name, IMUEvent::eventNames[(int)e.type], data, 3);
    }
    break;
    }
}

void MainManager::fileEvent(const FileEvent &e)
{
#ifdef HAS_FILES
    if (e.type == FileEvent::UploadStart)
    {
#ifdef LED_COUNT
        leds.setMode(LedManager::System);
        leds.sysLedMode.uploadFeedback = true;
#endif
        comm.oscManager.setEnabled(false);
    }
    else if (e.type == FileEvent::UploadProgress)
    {
#ifdef LED_COUNT
        leds.sysLedMode.showUploadProgress(e.data.floatValue());
#endif
    }
    else if (e.type == FileEvent::UploadComplete)
    {
#ifdef LED_COUNT
        leds.sysLedMode.uploadFeedback = false;
        leds.setMode(LedManager::Stream);
#endif
        comm.oscManager.setEnabled(true);
    }
#endif
}

void MainManager::timerEvent(const TimerEvent &e)
{
    NDBG("Timer Event, init fileSystem and IMU");
    files.init();

    imu.init();
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
