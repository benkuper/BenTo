Preferences Settings::prefs;
DynamicJsonDocument Settings::settings(32000);

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

    return true;
}

bool Settings::saveSettings()
{

    size_t settingsSize = measureMsgPack(settings);
    char *bytes = (char *)malloc(settingsSize);
    size_t s = serializeMsgPack(settings, bytes, settingsSize);

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