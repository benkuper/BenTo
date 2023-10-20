#include "UnityIncludes.h"
#include "RootComponent.h"

ImplementSingleton(RootComponent);

bool RootComponent::initInternal(JsonObject)
{
    BoardInit;

    timeAtStart = millis();
    timeAtShutdown = 0;

    // parameters.clear(); // remove enabled in root component
    Settings::loadSettings();
    JsonObject o = Settings::settings.as<JsonObject>();

    AddOwnedComponent(&comm);
    AddStringParam(deviceName);
    AddStringParamConfig(deviceType);
    AddIntParamConfig(wakeUpButton);
    AddBoolParamConfig(wakeUpState);

#ifdef USE_LEDSTRIP
    AddOwnedComponent(&strips);
#endif

#ifdef USE_BATTERY
    AddOwnedComponent(&battery);
#endif

#ifdef USE_FILES
    AddOwnedComponent(&files);
#endif

#if USE_SCRIPT
    AddOwnedComponent(&script);
#endif

#ifdef USE_SERVER
    AddOwnedComponent(&server);
#endif

#ifdef USE_STREAMING
    AddOwnedComponent(&streamReceiver);
#endif

#ifdef USE_IO
    memset(IOComponent::availablePWMChannels, true, sizeof(IOComponent::availablePWMChannels));
    AddOwnedComponent(&ios);
#ifdef USE_BUTTON
    AddOwnedComponent(&buttons);
#endif
#endif

#if USE_MOTION
    AddOwnedComponent(&motion);
#endif

#if USE_SERVO
    AddOwnedComponent(&servo);
#endif

#if USE_STEPPER
    AddOwnedComponent(&stepper);
#endif

#ifdef USE_BEHAVIOUR
    AddOwnedComponent(&behaviours);
#endif

#ifdef USE_DUMMY
    AddOwnedComponent(&dummies);
#endif

#ifdef USE_SEQUENCE
    AddOwnedComponent(&sequence);
#endif

#ifdef USE_WIFI
    AddOwnedComponent(&wifi);
#endif

    return true;
}

void RootComponent::updateInternal()
{
    timer.tick();
}

void RootComponent::restart()
{
    clear();
    delay(500);
    ESP.restart();
}

void RootComponent::shutdown()
{
    timeAtShutdown = millis();
    timer.in(1000, [](void *) -> bool
             {  RootComponent::instance->powerdown(); return false; });
}

void RootComponent::powerdown()
{
    clear();

    // NDBG("Sleep now, baby.");

    delay(500);

    if (wakeUpButton > 0)
        esp_sleep_enable_ext0_wakeup((gpio_num_t)wakeUpButton, wakeUpState);

        // #elif defined TOUCH_WAKEUP_PIN
        //     touchAttachInterrupt((gpio_num_t)TOUCH_WAKEUP_PIN, touchCallback, 110);
        //     esp_sleep_enable_touchpad_wakeup();
        // #endif

#ifdef ESP8266
    ESP.deepSleep(5e6);
#else
    esp_deep_sleep_start();
#endif
}

void RootComponent::saveSettings()
{
    Settings::settings.clear();
    JsonObject o = Settings::settings.to<JsonObject>();
    fillSettingsData(o, true);
    Settings::saveSettings();
}

void RootComponent::clearSettings()
{
    Settings::clearSettings();
    NDBG("Settings cleared, will reboot now.");
    delay(500);
    restart();
}

void RootComponent::onChildComponentEvent(const ComponentEvent &e)
{
    if (isShuttingDown())
        return;

    if (e.component == &comm
#ifdef USE_BEHAVIOUR
        || e.component == &behaviours
#endif
    )
    {
        if ((e.component == &comm && e.type == CommunicationComponent::MessageReceived)
#ifdef USE_BEHAVIOUR
            || (e.component == &behaviours && e.type == BehaviourManagerComponent::CommandLaunched)
#endif
        )
        {
            if (Component *targetComponent = getComponentWithName(e.data[0].stringValue()))
            {
                bool handled = targetComponent->handleCommand(e.data[1], &e.data[2], e.numData - 2);
                if (!handled)
                    NDBG("Command was not handled " + e.data[0].stringValue() + " > " + e.data[1].stringValue());
            }
            else
            {
                NDBG("No component found for " + e.data[0].stringValue());
            }

            return;
        }
    }
#ifdef USE_WIFI
    else if (e.component == &wifi)
    {
        if (e.type == WifiComponent::ConnectionStateChanged)
        {
            if (wifi.state == WifiComponent::Connected)
            {
                NDBG("Setup connections now");
                server.setupConnection();

#if USE_STREAMING
                streamReceiver.setupConnection();
#endif

#if USE_OSC
                comm.osc.setupConnection();
#endif
            }
        }
    }
#endif

#if USE_BATTERY
    else if (e.component == &battery)
    {
        if (e.type == BatteryComponent::CriticalBattery)
        {
#if USE_LEDSTRIP
            strips.items[0]->setBrightness(.05f);
#endif
            shutdown();
        }
    }
#endif

    comm.sendEventFeedback(e);
}

void RootComponent::childParamValueChanged(Component *caller, Component *comp, void *param)
{
#if USE_BUTTON
    if (caller == &buttons)
    {
        ButtonComponent *bc = (ButtonComponent *)comp;
        // DBG("Root param value changed " + bc->name+" > "+String(param == &bc->veryLongPress) + " / " + String(bc->veryLongPress)+" can sd : "+String(bc->canShutDown)+" / "+String(buttons.items[0]->canShutDown));
        if (param == &bc->veryLongPress && bc->veryLongPress && bc->canShutDown)
            shutdown();
    }
#endif
}

bool RootComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if (command == "shutdown")
        shutdown();
    else if (command == "restart")
        restart();
    else if (command == "stats")
    {
        DBG("Heap " + String(ESP.getFreeHeap()) + " free / " + String(ESP.getHeapSize()) + " total");
        DBG("Free Stack size  " + String((int)uxTaskGetStackHighWaterMark(NULL)) + " free");
        // comm->sendMessage(this, "freeHeap", String(ESP.getFreeHeap()) + " bytes");
    }
    else if (command == "saveSettings")
    {
        saveSettings();
    }
    else if (command == "showSettings")
    {
        String test;
        serializeJson(Settings::settings, test);
        DBG(test);
    }
    else if (command == "factoryReset")
    {
        clearSettings();
    }
    else
    {
        return false;
    }

    return true;
}

String RootComponent::getDeviceID() const
{
    byte mac[6]{0, 0, 0, 0, 0, 0};
#ifdef USE_WIFI
    WiFi.macAddress(mac);
#endif

    String d = "";
    for (int i = 0; i < 6; i++)
        d += (i > 0 ? "-" : "") + String(mac[i], HEX);

    d.toUpperCase();
    return d;
}