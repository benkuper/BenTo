#pragma once

#include "wifi/WifiManager.h"
#include "serial/SerialManager.h"
#include "osc/OSCManager.h"

class CommunicationEvent
{
public:
    enum Type { MessageReceived };
    CommunicationEvent(Type type) : type(type) {}
    CommunicationEvent(Type type, String target, String data) :
        type(type),target(target),data(data)
    {}

    Type type;
    String target;
    String data;
};


class CommunicationManager :
    public Component,
    public EventBroadcaster<CommunicationEvent>
{
public:
    CommunicationManager();
    ~CommunicationManager(){}

    WifiManager wifiManager;
    SerialManager serialManager;
    OSCManager oscManager;
    
    void init();
    void update();

    void serialMessageEvent(const SerialEvent& e);
    void wifiConnectionEvent(const WifiManagerEvent& e);
    void oscMessageEvent(const OSCEvent& e);
};