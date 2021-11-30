#pragma once

#include <Preferences.h>

DeclareComponentSingleton(Settings, "settings",)

    #ifdef ESP32
    Preferences prefs;
    #endif

    void initInternal() override
    {
        #ifdef ESP32
        prefs.begin("bentuino");
        #endif
    }

    void updateInternal()
    {

    }

    void clearInternal()
    {

    }

    float getFloat(Component * c, const String &name, const float &defaultValue = 0)
    {
        #ifdef ESP32
        return prefs.getFloat((c->name+"."+name).c_str(), defaultValue);
        #endif

        return defaultValue;
    }

    int getInt(Component * c, const String &name, const int &defaultValue = 0)
    {
        #ifdef ESP32
        return prefs.getInt((c->name+"."+name).c_str(), defaultValue);
        #endif

        return defaultValue;
    }

    bool getBool(Component * c, const String &name, const bool &defaultValue = false)
    {
        #ifdef ESP32
        return prefs.getBool((c->name+"."+name).c_str(), defaultValue);
        #endif

        return defaultValue;
    }

    String getString(Component * c, const String &name, const String &defaultValue = "")
    {
        #ifdef ESP32
        return prefs.getString((c->name+"."+name).c_str(), defaultValue.c_str());
        #endif

        return defaultValue;
    }

EndDeclareComponent