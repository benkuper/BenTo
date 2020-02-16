#include "OSCManager.h"

OSCManager::OSCManager() : 
    Component("osc"),
    enabled(false)
{
}

void OSCManager::init()
{
    setEnabled(true);
}

void OSCManager::update()
{
    if (!enabled)
        return;

    receiveOSC();
}

void OSCManager::setEnabled(bool value)
{
    if (enabled == value)
        return;

    enabled = value;

    if (enabled)
    {
        udp.begin(localPort);
        udp.flush();
    }
    else
    {
        udp.flush();
        udp.stop();
    }
}

void OSCManager::receiveOSC()
{
    OSCMessage msg;
    int size;
    if ((size = udp.parsePacket()) > 0)
    {
        while (size--) msg.fill(udp.read());
        if (!msg.hasError())  sendEvent(OSCEvent(msg));
    }
}

void OSCManager::sendMessage(OSCMessage msg)
{
    if (remoteHost.length() == 0)
        return;

    char addr[32];
    msg.getAddress((char *)remoteHost.c_str());

    NDBG("Send OSC message " + String(addr));
    udp.beginPacket((char *)remoteHost.c_str(), (uint16_t)remotePort);
    msg.send(udp);
    udp.endPacket();
    msg.empty();
}

void OSCManager::sendMessage(String address)
{
    sendMessage(OSCMessage(address.c_str()));
}
