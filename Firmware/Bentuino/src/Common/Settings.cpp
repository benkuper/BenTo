Preferences Settings::prefs;
JsonDoc Settings::settings;

bool Settings::loadSettings()
{
    prefs.begin("bentuino");

    size_t settingsSize = prefs.getBytesLength("settings");
    char *bytes = (char *)malloc(settingsSize);
    prefs.getBytes("settings", bytes, settingsSize);

    DeserializationError err = deserializeMsgPack(settings, bytes);
    if (err)
    {
        DBG("Loading settings.json error : " + String(err.c_str()));
        return false;
    }

    settings["test"] = "super";
    DBG("Settings loaded "+String(settingsSize));

    return true;
}

bool Settings::saveSettings()
{
    DBG("Save settings");
    size_t settingsSize = measureMsgPack(settings);
    DBG("malloc");
    char *bytes = (char *)malloc(settingsSize);
    DBG("serialize");
    size_t s = serializeMsgPack(settings, bytes, settingsSize);

    DBG("Test here");
    String test;
    serializeJson(settings, test);
    DBG("SETTINGS Json serialized and packed : " + String(settingsSize));
    DBG(test);

    prefs.clear();
    if (s == 0)
    {
        DBG("Saving settings error");
        return false;
    }

    // if(prefs.isKey("settings.json")) prefs.remove("settings.json");
    prefs.putBytes("settings", bytes, s);

    DBG("Settings saved.");
    return true;
}

bool Settings::clearSettings()
{
    prefs.clear();
    settings.clear();
    DBG("Settings cleared.");
    return true;
}

var Settings::getVal(JsonObject o, const String &name, var defaultVal)
{
    // DBG("Get val " + name + "/" + String((int)o.containsKey(name)));

    if (!o.containsKey(name))
        return defaultVal;

    JsonVariant val = o[name].containsKey("value") ? o[name]["value"].as<JsonVariant>() : o[name].as<JsonVariant>();

    switch (defaultVal.type)
    {
    case 'b':
        return val.as<bool>();
    case 'i':
        return val.as<int>();
    case 'f':
        return val.as<float>();
    case 's':
        return val.as<String>();
    }

    NDBG("Type not found " + defaultVal.type);
    return var();
}