#pragma once

#include <Arduino.h>
#include "DebugHelpers.h"

struct var
{
    char type;
    union
    {
        int i;
        float f;
        char * s;
    } value;

    int intValue() const
    {
        switch (type)
        {
        case 'i':
            return value.i;
        case 'f':
            return (int)value.f;
        case 's':
            return String(value.s).toInt();
        }
        return 0;
    }

    float floatValue() const
    {
        switch (type)
        {
        case 'i':
            return (float)value.i;
        case 'f':
            return value.f;
        case 's':
            return String(value.s).toFloat();
        }

        return 0;
    }

    String stringValue() const
    {
        switch (type)
        {
        case 'i':
            return String(value.i);
        case 'f':
            return String(value.f);
        case 's':
            return value.s;
        }
        return "";
    }
};

class Component
{
public:
    Component(const String &name);
    virtual ~Component() {}

    String name;

    static Component * registeredComponents[64];
    static int numRegisteredComponents;
    static Component * getComponentForName(String _name);

    virtual bool handleCommand(String command, var * data, int numData) { return false; }
    bool checkCommand(const String &command, const String &checkCommand, int numData, int expected);
};
