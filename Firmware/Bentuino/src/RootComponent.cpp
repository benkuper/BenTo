ImplementSingleton(RootComponent)

    bool RootComponent::initInternal()
{
    BoardInit;

    // Generate deviceID
    byte mac[6];
    WiFi.macAddress(mac);

    deviceID = "";
    for (int i = 0; i < 6; i++)
        deviceID += (i > 0 ? "-" : "") + String(mac[i], HEX);

    deviceID.toUpperCase();

    AddComponent(settings, Settings);
    AddComponent(comm, Communication);
    AddComponent(wifi, Wifi);
    AddComponent(battery, Battery);
    AddComponent(files, Files);
    AddComponent(server, WebServer);
    AddComponent(sequence, Sequence);

    AddComponent(strip, LedStrip);
    AddComponent(imu, IMU);

    return true;
}

void RootComponent::restart()
{
    ESP.restart();
}

void RootComponent::shutdown()
{
    clear();

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

void RootComponent::onChildComponentEvent(const ComponentEvent &e)
{
    if(e.component == comm)
    {
        if(e.type == CommunicationComponent::MessageReceived)
        {
            
            if(Component * targetComponent = getComponentWithName(e.data[0].stringValue()))
            {
                targetComponent->handleCommand(e.data[1], &e.data[2], e.numData-2);
            }else{
                NDBG("No component found for "+e.data[0].stringValue());
            }
            
            return;
        }
    }else if(e.component == wifi)
    {
        if(e.type == WifiComponent::ConnectionStateChanged)
        {
            comm->osc->enabled->set((bool)(wifi->state == WifiComponent::Connected), true);
            server->enabled->set((bool)(wifi->state == WifiComponent::Connected), true);
        }
    }

    comm->sendEventFeedback(e);
}

bool RootComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if(command == "shutdown") shutdown();
    else if(command == "restart") restart();
    else
    {
        return false;
    }

    return true;
}
