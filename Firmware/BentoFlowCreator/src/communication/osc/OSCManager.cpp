#include "OSCManager.h"
#include "../wifi/WifiManager.h"

OSCManager::OSCManager(WifiManager * wifi) : Component("osc"),
                           wifi(wifi),    
                           enabled(false),
                           pingTimeout(5000),
                           isAlive(false)
{
}

void OSCManager::init()
{
    prefs.begin("osc", true);
    remoteHost = prefs.getString("remoteHost", "");
    prefs.end();
    
    NDBG("Init");
}

void OSCManager::update()
{
    if (!enabled)
        return;

    if(pingEnabled && millis() > timeSinceLastReceivedPing + pingTimeout)
    {
        setAlive(false);
    }

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
        setAlive(true);
        timeSinceLastReceivedPing = millis();
    }
    else
    {
        udp.flush();
        udp.stop();
        setAlive(false);
    }   
}


void OSCManager::setAlive(bool value) 
{
    if(isAlive == value) return;
    isAlive = value;
    NDBG("Set alive "+String(isAlive));
    sendEvent(OSCEvent(OSCEvent::AliveChanged));
}


void OSCManager::receiveOSC()
{
    OSCMessage msg;
    int size;
    if ((size = udp.parsePacket()) > 0)
    {
        while (size--)
            msg.fill(udp.read());
        if (!msg.hasError())
            processMessage(msg);
    }
}

void OSCManager::processMessage(OSCMessage &msg)
{
    if (msg.match("/yo"))
    {
        char hostData[32];
        msg.getString(0, hostData, 32);
        remoteHost = String(hostData);
        NDBG("Got yo request from : " + remoteHost);

        prefs.begin("osc");
        prefs.putString("remoteHost", remoteHost);
        prefs.end();

        OSCMessage msg("/wassup");
        
        msg.add(wifi->getIP().c_str());
        msg.add(getDeviceID().c_str());
        msg.add(DEVICE_TYPE);

        sendMessage(msg);
    }
    else if (msg.match("/ping"))
    {
        setAlive(true);
        pingEnabled = true;
        timeSinceLastReceivedPing = millis();
        OSCMessage msg("/pong");
        msg.add(getDeviceID().c_str());
        sendMessage(msg);
    }else
    {
        sendEvent(OSCEvent(OSCEvent::MessageReceived, &msg));
    }
}

void OSCManager::sendMessage(OSCMessage &msg)
{
    if(!enabled) return;

    if (remoteHost.length() == 0)
        return;

    char addr[32];
    msg.getAddress(addr);
    //NDBG("Send OSC message to "+ remoteHost + " : " + String(addr));
    udp.beginPacket((char *)remoteHost.c_str(), (uint16_t)remotePort);
    msg.send(udp);
    udp.endPacket();
    msg.empty();
}

void OSCManager::sendMessage(String address)
{
    if(!enabled) return;

    OSCMessage m(address.c_str());
    sendMessage(m);
}

void OSCManager::sendMessage(String source, String command, var *data, int numData)
{
    if(!enabled) return;

    OSCMessage msg(("/"+source+"/"+command).c_str());
    msg.add(getDeviceID().c_str());
    for(int i=0;i<numData;i++)
    {
        switch(data[i].type)
        {
            case 'f': msg.add(data[i].floatValue());
            break;

            case 'i': msg.add(data[i].intValue());
            break;

            case 's': msg.add(data[i].stringValue().c_str());
            break;
        }
    } 
    sendMessage(msg);
}

void OSCManager::handleMessage(String command, var * data, int numData)
{

    if(checkCommand(command, "enabled", numData, 1))
    {
        setEnabled(data[0].intValue() == 1);
        return true;
    }

    return false;
}