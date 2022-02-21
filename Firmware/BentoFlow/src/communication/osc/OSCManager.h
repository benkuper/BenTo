#pragma once

#include "../../common/Common.h"

#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBoards.h>

#ifdef ESP32
#include <ESPmDNS.h>
#elif defined ESP8266
#include <ESP8266mDNS.h>
#endif

class OSCEvent
{
public:
    enum Type
    {
        MessageReceived,
        AliveChanged
    };

    OSCEvent(Type t, OSCMessage *m = nullptr) : type(t), msg(m) {}

    Type type;
    OSCMessage *msg;
    OSCMessage &getMessage() { return *msg; }
};

class WifiManager; //forward declaration

class OSCManager : public Component, public EventBroadcaster<OSCEvent>
{
public:
    OSCManager(WifiManager *wifi);
    ~OSCManager() {}

    WifiManager *wifi;

    bool enabled;

#ifdef USE_PREFERENCES
    Preferences prefs;
#elif defined USE_SETTINGS_MANAGER
    SettingsManager prefs;
#endif

    WiFiUDP udp;

    const int localPort = 9000; // input port
    String remoteHost;
    const int remotePort = 10000; // output port

    long timeSinceLastReceivedPing;
    long pingTimeout;
    bool pingEnabled; //only activate ping check if received a first ping

    bool isAlive;

    void init();
    void update();

    void setEnabled(bool value);
    void setAlive(bool value);

    void receiveOSC();
    void processMessage(OSCMessage &m);

    void saveRemoteHost(String ip);

    void sendMessage(OSCMessage &m);
    void sendMessage(String address);
    void sendMessage(String source, String command, var *data, int numData);

    bool handleCommand(String command, var *data, int numData) override;
};