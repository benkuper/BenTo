#pragma once

#include <Arduino.h>
#include "DebugHelpers.h"

class Component
{
public:
    Component(const String &name) : name(name) {}
    virtual ~Component() {}
    String name;
};