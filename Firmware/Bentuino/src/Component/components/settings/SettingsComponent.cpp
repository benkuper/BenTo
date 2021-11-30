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
        return false;
    }

    prefs.putString("settings.json", json);

    NDBG(F("Settings saved."));
    return true;
}

void SettingsComponent::setConfig(const String &comp, const String &name, var val, bool save)
{
    NDBG("Set Config " + comp + ":" + name + " = " + val.stringValue());
    switch (val.type)
    {
    case 'b':
        settings[comp][name] = val.boolValue();
        break;
     case 'i':
        settings[comp][name] = val.intValue();
        break;
     case 'f':
        settings[comp][name] = val.floatValue();
        break;
     case 's':
        settings[comp][name] = val.stringValue();
        break;
    }

    if (save)
        saveSettings();
}
