#pragma once

#include "wifi/WifiManager.h"
#include "serial/SerialManager.h"
#include "osc/OSCManager.h"

class CommunicationEvent
{
public:
    enum Type
    {
        MessageReceived,
        TYPES_MAX
    };
    const String typeNames[TYPES_MAX]{"MessageReceived"};

    CommunicationEvent(Type type, String source, String target, var *data, int numData) : 
        type(type), source(source), target(target), data(data), numData(numData)
    {
    }

    Type type;
    String source;
    String target;
    var *data;
    int numData;

    int getInt(int index) const
    {
        if (index < 0 || index >= numData)
            return 0;

        switch (data[index].type)
        {
        case 'i':
            return data[index].value.i;
        case 'f':
            return (int)data[index].value.f;
        case 's':
            return String(data[index].value.s).toInt();
        }
        return 0;
    }

    float getFloat(int index) const
    {
        if (index < 0 || index >= numData)
            return 0;

        switch (data[index].type)
        {
        case 'i':
            return (float)data[index].value.i;
        case 'f':
            return data[index].value.f;
        case 's':
            return String(data[index].value.s).toFloat();
        }

        return 0;
    }

    String getString(int index) const
    {
        if (index < 0 || index >= numData)
            return "";

        switch (data[index].type)
        {
        case 'i':
            return String(data[index].value.i);
        case 'f':
            return String(data[index].value.f);
        case 's':
            return data[index].value.s;
        }
        return "";
    }

    String toString() const
    {
        String s = "*" + typeNames[type] + "* from " + source + " to " + target + " : ";
        for (int i = 0; i < numData; i++)
            s += (i > 0 ? ", " : "") + getString(i)+ "("+data[i].type+")";
        return s;
    }
};

class CommunicationManager : public Component,
                             public EventBroadcaster<CommunicationEvent>
{
public:
    CommunicationManager();
    ~CommunicationManager() {}

    WifiManager wifiManager;
    SerialManager serialManager;
    OSCManager oscManager;

    void init();
    void update();

    void serialMessageEvent(const SerialEvent &e);
    void wifiConnectionEvent(const WifiManagerEvent &e);
    void oscMessageEvent(const OSCEvent &e);
};