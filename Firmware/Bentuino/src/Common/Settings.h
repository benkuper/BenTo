#pragma once

#include <Preferences.h>

class Settings
{
public:
    static Preferences prefs;
    static DynamicJsonDocument settings;

    static bool loadSettings();
    static bool saveSettings();

    static var getVal(JsonObject o, const String& name, var defaultVal);
};