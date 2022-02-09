Preferences Settings::prefs;
DynamicJsonDocument Settings::settings(8000);

bool Settings::loadSettings()
{
    prefs.begin("bentuino");

    String json = prefs.getString("settings.json", "{}");

    DeserializationError err = deserializeJson(settings, json);
    if (err)
    {
        DBG("Loading settings.json error : " + String(err.c_str()));
        return false;
    }

    return true;
}

bool Settings::saveSettings()
{
    String json = "";
    size_t s = serializeJson(settings, json);

    prefs.clear();
    if (s == 0)
    {   
        DBG(F("Saving settings error"));
        return false;
    }

    prefs.putString("settings.json", json);

    DBG("Settings saved.");
    return true;
}

var Settings::getVal(JsonObject o, const String &name, var defaultVal)
{
    //DBG("Get val " + name + "/" + String((int)o.containsKey(name)));

    if (!o.containsKey(name))
        return defaultVal;

    switch (defaultVal.type)
    {
    case 'b':
        return o[name]["value"].as<bool>();
    case 'i':
        return o[name]["value"].as<int>();
    case 'f':
        return o[name]["value"].as<float>();
    case 's':
        return o[name]["value"].as<String>();
    }

    NDBG("Type not found "+defaultVal.type);
    return var();
}