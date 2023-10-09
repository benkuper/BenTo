#pragma once

#include <Preferences.h>

typedef StaticJsonDocument<6000> JsonDoc;

class Settings
{
public:
    static Preferences prefs;
    static JsonDoc settings;
    
    static bool loadSettings();
    static bool saveSettings();
    static bool clearSettings();

    static var getVal(JsonObject o, const String& name, var defaultVal);
};