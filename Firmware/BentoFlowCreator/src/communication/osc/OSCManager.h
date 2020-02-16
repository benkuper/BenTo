#pragma once

#include "../../common/Common.h"

#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBoards.h>

class OSCEvent
{
public:
    OSCEvent(OSCMessage *m) : msg(m) {}
    OSCMessage * msg;
    OSCMessage &getMessage() {return *msg;}
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
    void processMessage(OSCMessage &m);

    void sendMessage(OSCMessage &m);
    void sendMessage(String address);
};