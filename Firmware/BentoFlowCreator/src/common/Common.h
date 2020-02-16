//This file includes all common classes and definitions so it's the only one to include from other files

#pragma once

#include "Component.h"
#include "events/EventBroadcaster.h"

struct var
{
    char type;
    union
    {
        int i;
        float f;
        char * s;
    } value;
};