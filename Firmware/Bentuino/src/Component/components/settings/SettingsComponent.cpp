ImplementSingleton(SettingsComponent);

bool SettingsComponent::initInternal(JsonObject o)
{
    AddIntParam(propID);
    AddStringParam(deviceName);
    AddStringParamConfig(deviceType);
#ifdef USE_POWER
    AddIntParamConfig(wakeUpButton);
    AddBoolParamConfig(wakeUpState);
#endif
    return true;
}

bool SettingsComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if (command == "save")
    {
        saveSettings();
    }
    else if (command == "show")
    {
        String test;
        serializeJson(Settings::settings, test);
        DBG(test);
    }
    else if (command == "clear")
    {
        clearSettings();
    }
}

void SettingsComponent::saveSettings()
{
    Settings::settings.clear();
    JsonObject o = Settings::settings.to<JsonObject>();
    RootComponent::instance->fillSettingsData(o, true);
    Settings::saveSettings();
    NDBG("Settings saved");
}

void SettingsComponent::clearSettings()
{
    Settings::clearSettings();
    NDBG("Settings cleared, will reboot now.");
    delay(500);
    RootComponent::instance->restart();
}

String SettingsComponent::getDeviceID() const
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