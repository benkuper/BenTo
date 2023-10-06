#include "UnityIncludes.h"

ImplementSingleton(RootComponent);

bool RootComponent::initInternal(JsonObject)
{
    BoardInit;

    timeAtShutdown = 0;

    Settings::loadSettings();
    JsonObject o = Settings::settings.as<JsonObject>();

    AddOwnedComponent(&comm);
    AddParameter(deviceName);

    AddOwnedComponent(&battery);
    AddOwnedComponent(&sequence);

    AddOwnedComponent(&wifi);
    AddOwnedComponent(&files);
    // AddOwnedComponent(&script);
    AddOwnedComponent(&server);
    AddOwnedComponent(&streamReceiver);

    for (int i = 0; i < LEDSTRIP_MAX_COUNT; i++)
    {
        strips[i].name = "strip" + String(i + 1);
        AddOwnedComponent(&strips[i]);
    }
    for (int i = 0; i < BUTTON_MAX_COUNT; i++)
    {
        buttons[i].name ="button" + String(i + 1);
        AddOwnedComponent(&buttons[i]);
    }

    for (int i = 0; i < IO_MAX_COUNT; i++)
    {
        ios[i].name ="io" + String(i + 1);
        AddOwnedComponent(&ios[i]);
    }
#if USE_IMU
    AddOwnedComponent(&imu);
#endif

#if USE_SERVO
    AddOwnedComponent(&servo);
#endif

#if USE_STEPPER
    AddOwnedComponent(&stepper);
#endif

    return true;
}

void RootComponent::updateInternal()
{
    timer.tick();
}

void RootComponent::restart()
{
    // saveSettings();

    ESP.restart();
}

void RootComponent::shutdown()
{
    // saveSettings();

    timeAtShutdown = millis();
    timer.in(1000, [](void *) -> bool
             {  RootComponent::instance->powerdown(); return false; });
}

void RootComponent::powerdown()
{
    clear();

    // NDBG("Sleep now, baby.");

#ifdef POWER_WAKEUP_BUTTON
    esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKEUP_BUTTON, WAKEUP_BUTTON_STATE);
#elif defined TOUCH_WAKEUP_PIN
    touchAttachInterrupt((gpio_num_t)TOUCH_WAKEUP_PIN, touchCallback, 110);
    esp_sleep_enable_touchpad_wakeup();
#endif

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

    if (e.component == &comm)
    {
        if (e.type == CommunicationComponent::MessageReceived)
        {
            if (Component *targetComponent = getComponentWithName(e.data[0].stringValue()))
            {
                bool handled = targetComponent->handleCommand(e.data[1], &e.data[2], e.numData - 2);
                if (!handled)
                    NDBG("Command was not handled " + e.data[0].stringValue() + "." + e.data[1].stringValue());
            }
            else
            {
                NDBG("No component found for " + e.data[0].stringValue());
            }

            return;
        }
    }
    else if (e.component == &wifi)
    {
        if (e.type == WifiComponent::ConnectionStateChanged)
        {
            comm.osc.setupConnection();
            server.setupConnection();
            streamReceiver.setupConnection();
        }
    }
    // else if (e.component == &buttons[0])
    // {
    // Should move to behaviour system
    //  if (button->isSystem.boolValue())
    //  {
    //      switch (e.type)
    //      {
    //      case ButtonComponent::ShortPress:
    //      {
    //          if (wifi.state == WifiComponent::Connecting)
    //              wifi.disable();
    //      }
    //      break;

    //     case ButtonComponent::VeryLongPress:
    //         shutdown();
    //         break;
    //     }
    // }
    // }

    comm.sendEventFeedback(e);
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
    byte mac[6];
    WiFi.macAddress(mac);

    String d = "";
    for (int i = 0; i < 6; i++)
        d += (i > 0 ? "-" : "") + String(mac[i], HEX);

    d.toUpperCase();
    return d;
}