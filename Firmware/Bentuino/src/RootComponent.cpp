ImplementSingleton(RootComponent);

bool RootComponent::initInternal(JsonObject)
{
    BoardInit;

    timeAtShutdown = 0;

    // Generate deviceID
    byte mac[6];
    WiFi.macAddress(mac);

    deviceID = "";
    for (int i = 0; i < 6; i++)
        deviceID += (i > 0 ? "-" : "") + String(mac[i], HEX);

    deviceID.toUpperCase();

    Settings::loadSettings();
    JsonObject o = Settings::settings.as<JsonObject>();

    AddComponent("comm", comm, Communication, true);

    AddComponent("streamReceiver", streamReceiver, LedStreamReceiver, false);
    AddComponent("strip", strip, LedStrip, true);

    AddComponent("battery", battery, Battery, true);
    AddComponent("sequence", sequence, Sequence, true);

    AddComponent("server", server, WebServer, false);
    AddComponent("imu", imu, IMU, false);

    AddComponent("wifi", wifi, Wifi, true);
    AddComponent("files", files, Files, true);

    AddComponent("button", button, Button, false);

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

    NDBG("Sleep now, baby.");

#ifdef WAKEUP_BUTTON
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
    NDBG("Saving settings");
    Settings::settings.clear();
    JsonObject o = Settings::settings.to<JsonObject>();
    fillSettingsData(o, true);
    NDBG("Settings retrieved");
    Settings::saveSettings();
}

void RootComponent::onChildComponentEvent(const ComponentEvent &e)
{
    if (isShuttingDown())
        return;

    if (e.component == comm)
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
    else if (e.component == wifi)
    {
        if (e.type == WifiComponent::ConnectionStateChanged)
        {
            bool isConnected = wifi->state == WifiComponent::Connected;
            comm->osc->enabled->set(isConnected);
            server->enabled->set(isConnected);
            streamReceiver->enabled->set(isConnected);
        }
    }
    else if (e.component == button)
    {
        if (button->isSystem->boolValue())
        {
            switch (e.type)
            {
            case ButtonComponent::ShortPress:
            {
                if (wifi->state == WifiComponent::Connecting)
                    wifi->disable();
            }
            break;

            case ButtonComponent::VeryLongPress:
                shutdown();
                break;
            }
        }
    }

    comm->sendEventFeedback(e);
}

bool RootComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if (command == "shutdown")
        shutdown();
    else if (command == "restart")
        restart();
    else if (command == "stats")
    {
        comm->sendMessage(this, "freeHeap", String(ESP.getFreeHeap()) + " bytes");
    }
    else if (command == "saveSettings")
    {
        saveSettings();
    }
    else
    {
        return false;
    }

    return true;
}
