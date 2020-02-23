#pragma once
#include "battery/BatteryManager.h"
#include "button/ButtonManager.h"
#include "imu/IMUManager.h"

class SensorEvent
{
public:
    enum Type
    {
        BatteryUpdate,
        OrientationUpdate,
        ButtonUpdate,
        TouchUpdate,
        TYPES_MAX
    };

    const String typeNames[TYPES_MAX]{"BatteryUpdate", "OrientationUpdate","ButtonUpdate","TouchUpdate"};

    SensorEvent(Type type, String source, var *data, int numData) : 
        type(type), source(source), data(data), numData(numData)
    {
    }

    Type type;
    String source;
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

    BatteryManager batteryManager;
    ButtonManager btManager;

    IMUManager imuManager;

    void init(bool initIMU = false);
    void update();

    void batteryEvent(const BatteryEvent &e);
    void buttonEvent(const ButtonEvent &e);

    void imuEvent(const IMUEvent &e);
};