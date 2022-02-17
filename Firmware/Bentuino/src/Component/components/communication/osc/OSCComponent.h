#pragma once

#define OSC_LOCAL_PORT 9000
#define OSC_REMOTE_PORT 10000
#define OSC_PING_TIMEOUT 5000

class OSCComponent : public Component
{
public:
    OSCComponent() : Component(Type_OSC) { instance = this; }
    ~OSCComponent() {}

    DeclareSingleton(OSCComponent);

    WiFiUDP udp;
    bool udpIsInit;
    Parameter *remoteHost;

    // Ping
    bool pingEnabled; // only activate ping check if received a first ping
    long timeSinceLastReceivedPing;
    Parameter *isAlive;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    void onEnabledChanged() override;

    void setupConnection();

    void receiveOSC();
    void processMessage(OSCMessage &m);

    void saveRemoteHost(String ip);

    void sendMessage(OSCMessage &m);
    void sendMessage(String address);
    void sendMessage(const String &source, const String &command, var *data, int numData);

    // Helper
    var OSCArgumentToVar(OSCMessage &m, int index);

    DeclareComponentEventTypes(MessageReceived);
    DeclareComponentEventNames("MessageReceived");
};