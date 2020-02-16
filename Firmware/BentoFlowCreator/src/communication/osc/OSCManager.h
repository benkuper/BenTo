#pragma once

#include "../../common/Component.h"
#include "../../common/events/EventBroadcaster.h"

#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBoards.h>

class OSCEvent
{
public:
    OSCEvent(OSCMessage m) : message(m) {}
    OSCMessage message;
};

class OSCManager :
    public Component,
    public EventBroadcaster<OSCEvent>
{
public:
    OSCManager();
    ~OSCManager() {}

    bool enabled;
    
    WiFiUDP udp;

    const int localPort = 9000;
    String remoteHost;
    const int remotePort = 10000;

    long timeSinceLastReceivedPing;
    long pingTimeout;

    void init();
    void update();

    void setEnabled(bool value);

    void receiveOSC();

    void sendMessage(OSCMessage m);
    void sendMessage(String address);
};