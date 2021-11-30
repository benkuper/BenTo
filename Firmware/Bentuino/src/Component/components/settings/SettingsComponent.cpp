ImplementSingleton(SettingsComponent);

bool SettingsComponent::initInternal()
{
#ifdef ESP32
    prefs.begin("bentuino");

    String json = prefs.getString("settings.json", "{}");
    DeserializationError err = deserializeJson(settings, json);
    if (err)
    {
        NDBG("Loading settings.json error : " + String(err.c_str()));
        return false;
    }
#endif

    return true;
}

bool SettingsComponent::saveSettings()
{
    String json = "";
    size_t s = serializeJson(settings, json);

    if (s == 0)
    {
        NDBG(F("Saving settings error"));
        json = "{}";
    }

    prefs.putString("settings.json", json);

    return true;
}
