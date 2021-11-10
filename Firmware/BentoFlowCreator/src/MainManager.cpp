#include "MainManager.h"

MainManager * MainManager::instance = nullptr;
MainManager::MainManager(String fwVersion) : Component("root"),
                                             fwVersion(fwVersion),
                                             initTimer(2000)
{
    instance = this;
}

void MainManager::init()
{

#ifdef HAS_DISPLAY
    display.init();
#endif

#ifdef GROUND_PIN_COUNT
    for (int i = 0; i < GROUND_PIN_COUNT; i++)
    {
        pinMode(groundPins[i], OUTPUT);
        digitalWrite(groundPins[i], LOW);
        gpio_hold_en((gpio_num_t)groundPins[i]);
    }
    gpio_deep_sleep_hold_en();
#endif

#ifdef HIGH_PIN_COUNT
    for (int i = 0; i < HIGH_PIN_COUNT; i++)
    {
        pinMode(highPins[i], OUTPUT);
        digitalWrite(highPins[i], HIGH);
        gpio_hold_en((gpio_num_t)highPins[i]);
    }
    gpio_deep_sleep_hold_en();
#endif



    ((EventBroadcaster<CommunicationEvent> *)&comm)->addListener(std::bind(&MainManager::communicationEvent, this, std::placeholders::_1));
    ((EventBroadcaster<ConnectionEvent> *)&comm)->addListener(std::bind(&MainManager::connectionEvent, this, std::placeholders::_1));
    comm.init();

    leds.init();

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

    touch.init();
    touch.addListener(std::bind(&MainManager::touchEvent, this, std::placeholders::_1));

    pwm.init();

    imu.addListener(std::bind(&MainManager::imuEvent, this, std::placeholders::_1));

    leds.rgbManager.addListener(std::bind(&MainManager::rgbLedsEvent, this, std::placeholders::_1));

    cap.init();
    cap.addListener(std::bind(&MainManager::capacitiveEvent, this, std::placeholders::_1));

    files.addListener(std::bind(&MainManager::fileEvent, this, std::placeholders::_1));

    scripts.init();

    initTimer.addListener(std::bind(&MainManager::timerEvent, this, std::placeholders::_1));

    NDBG("Board is "+String(ARDUINO_BOARD));
}

void MainManager::update()
{
    initTimer.update();

    files.update();
    
    if (files.isUploading)
        return;

    comm.update();

    imu.update();

    buttons.update();
    battery.update();
    touch.update();
    cap.update();

    //tmp call this from ledmanager
    //scripts.update();
    
    pwm.update();

// TSTART()
    leds.update();
// TFINISH("Leds")
    
#ifdef POWEROFF_IF_NOTCONNECTED
    if (comm.wifiManager.state == ConnectionState::ConnectionError)
    {
        if (!battery.isCharging)
        {
            NDBG("Not connected and battery not charging, poweroff");

            sleep(CRGB::Yellow);
        }
    }
#endif
}

void touchCallback() {}

void MainManager::sleep(CRGB color)
{
    NDBG("Sleep now ! ");
    imu.shutdown();

    comm.sendMessage(name, "sleep", nullptr, 0);
    leds.shutdown(color); // to replace with battery color
    pwm.shutdown();

    delay(500);

#ifdef SLEEP_PIN
    pinMode(SLEEP_PIN, OUTPUT);

#ifdef INFINITE_LOOP_ON_SLEEP
    while (true)
        digitalWrite(SLEEP_PIN, SLEEP_PIN_SLEEP_VAL);
#else
    digitalWrite(SLEEP_PIN, SLEEP_PIN_SLEEP_VAL);
#endif // INFINITE LOOP

#endif // SLEEP_PIN

#ifdef SLEEP_WAKEUP_BUTTON
    esp_sleep_enable_ext0_wakeup(SLEEP_WAKEUP_BUTTON, SLEEP_WAKEUP_STATE);
#else
#if defined TOUCH_WAKEUP_PIN
    // Configure Touchpad as wakeup source
    touchAttachInterrupt(TOUCH_WAKEUP_PIN, touchCallback, 110);
    esp_sleep_enable_touchpad_wakeup();
#endif
#endif

#ifdef ESP8266
    ESP.deepSleep(5e6);
#else
    esp_deep_sleep_start();
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

        if (e.type == Connected || e.type == ConnectionError || e.type == Disabled || e.type == Hotspot)
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
        NDBG("Critical battery level detected => sleeping");
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
        NDBG("Button " + String(e.id) + " " + String(e.value));
        break;

    case ButtonEvent::VeryLongPress:
    {
        NDBG("Very long press detected: sleeping");

#ifndef NO_SLEEP_BUTTON
#ifdef SLEEP_BUTTON_ID
        int sleepBTID = SLEEP_BUTTON_ID;
#else
        int sleepBTID = 0;
#endif

        if (e.id == sleepBTID)
            sleep(); // only first button can sleep
#endif
    }

    case ButtonEvent::MultiPress:
        if (comm.wifiManager.state == Disabled || comm.wifiManager.state == ConnectionError)
        {
            if (e.value == 2)
            {
                leds.playerMode.stop();
            }
            else if (e.value >= 3)
            {
                leds.playerMode.load("demo" + String(e.value - 3));
                leds.playerMode.loopShow = true;
                leds.playerMode.play();
            }
        }
        break;
    }
}

void MainManager::touchEvent(const TouchEvent &e)
{
    int numBTData = (e.type == TouchEvent::MultiPress || e.type == TouchEvent::Pressed) ? 2 : 1;
    var *data = (var *)malloc((numBTData) * sizeof(var));

    data[0].value.i = e.id;
    data[0].type = 'i';

    if (numBTData > 1)
    {
        data[1].value.i = e.value;
        data[1].type = 'i';
    }

    comm.sendMessage(touch.name, TouchEvent::eventNames[(int)e.type], data, numBTData);

    switch (e.type)
    {
    case TouchEvent::Pressed:
        if (e.value == 1)
        {
            if (comm.wifiManager.state == Connecting)
                comm.wifiManager.disable();
        }
        NDBG("Touch " + String(e.id) + " " + String(e.value));
        break;

    case TouchEvent::VeryLongPress:
    {

#ifndef NO_SLEEP_TOUCH
#ifdef SLEEP_TOUCH_ID
        int sleepBTID = SLEEP_BUTTON_ID;
#else
        int sleepBTID = 0;
#endif

        if (e.id == sleepBTID)
            sleep(); // only first button can sleep
#endif
    }

    case TouchEvent::MultiPress:
        if (comm.wifiManager.state == Disabled || comm.wifiManager.state == ConnectionError)
        {
            if (e.value == 2)
            {
                leds.playerMode.stop();
            }
            else if (e.value >= 3)
            {
                leds.playerMode.load("demo" + String(e.value - 3));
                leds.playerMode.loopShow = true;
                leds.playerMode.play();
            }
        }
        break;
    }
}

void MainManager::imuEvent(const IMUEvent &e)
{
    switch (e.type)
    {
    case IMUEvent::OrientationUpdate:
    case IMUEvent::LinearAccelUpdate:
    case IMUEvent::AccelUpdate:
    case IMUEvent::GyroUpdate:
    case IMUEvent::Gravity:
    case IMUEvent::CalibrationStatus:
    {
        var data[3];
        for (int i = 0; i < e.numData; i++)
        {
            data[i].type = 'f';
            data[i].value.f = e.data[i];
        }

        comm.sendMessage(imu.name, IMUEvent::eventNames[(int)e.type], data, 3);
    }
    break;

    case IMUEvent::ThrowState:
    {
        var data[1];
        data[0].type = 'i';
        data[0].value.i = imu.throwState;
        comm.sendMessage(imu.name, IMUEvent::eventNames[(int)e.type], data, 1);
    }
    break;
    }
}

void MainManager::capacitiveEvent(const CapacitiveEvent &e)
{
#ifdef CAPACITIVE_COUNT
    switch (e.type)
    {
    case CapacitiveEvent::ValuesUpdate:
    {
        var data[CAPACITIVE_COUNT];
        for (int i = 0; i < e.numData; i++)
        {
            data[i].type = 'f';
            data[i].value.f = e.data[i];
        }

        comm.sendMessage(cap.name, CapacitiveEvent::eventNames[(int)e.type], data, CAPACITIVE_COUNT);
    }
    break;

    case CapacitiveEvent::TouchUpdate:
    {
        var data[2];
        data[0].type = 'i';
        data[0].value.i = e.index;
        data[1].type = 'i';
        data[1].value.i = (int)e.value;

        comm.sendMessage(cap.name, CapacitiveEvent::eventNames[(int)e.type], data, 2);
    }
    break;
    }
#endif
}

void MainManager::rgbLedsEvent(const RGBLedsEvent &e)
{
    switch (e.type)
    {
    case RGBLedsEvent::BrightnessStatus:
    {
        var data[1];
        for (int i = 0; i < e.numData; i++)
        {
            data[i].type = 'f';
            data[i].value.f = e.data[i];
        }

        comm.sendMessage(leds.rgbManager.name, RGBLedsEvent::eventNames[(int)e.type], data, 1);
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
    else if (e.type == FileEvent::FileList)
    {
        var data[1];
        data[0].type = 's';
        data[0].value.s = e.data.value.s;

        comm.sendMessage(files.name, FileEvent::eventNames[(int)e.type], data, 1);
        NDBG("FileList Event in Main Manager");
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
