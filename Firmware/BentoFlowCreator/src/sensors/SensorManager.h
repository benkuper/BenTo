#pragma once
#include "button/ButtonManager.h"

class SensorEvent
{
public:
    enum Type
    {
        OrientationUpdate,
        ButtonUpdate,
        TouchUpdate,
        TYPES_MAX
    };

    const String typeNames[TYPES_MAX]{"OrientationUpdate","ButtonUpdate","TouchUpdate"};

    SensorEvent(Type type, var *data, int numData) : 
        type(type), data(data), numData(numData)
    {
    }

    Type type;
    var *data;
    int numData;

    String toString() const
    {
        String s = "*" + typeNames[type] + ", data : ";
        for (int i = 0; i < numData; i++)
            s += (i > 0 ? ", " : "") + data[i].stringValue()+ "("+data[i].type+")";
        
       return s;
    }
};

class SensorManager :
    public Component,
    public EventBroadcaster<SensorEvent>
{
public:
    SensorManager();
    ~SensorManager(){}

    ButtonManager btManager;
    
    void init();
    void update();

    void buttonEvent(const ButtonEvent &e);
};