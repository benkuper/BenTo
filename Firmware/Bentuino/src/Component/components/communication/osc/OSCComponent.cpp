ImplementSingleton(OSCComponent)

    bool OSCComponent::initInternal(JsonObject o)
{
    udpIsInit = false;

    pingEnabled = false;
    timeSinceLastReceivedPing = 0;

    AddAndSetParameter(remoteHost);
    AddParameter(isAlive);

    return true;
}

void OSCComponent::updateInternal()
{
    if (pingEnabled && millis() > timeSinceLastReceivedPing + OSC_PING_TIMEOUT)
        isAlive.set(false);

    receiveOSC();
}

void OSCComponent::clearInternal()
{
}

void OSCComponent::onEnabledChanged()
{
    setupConnection();
}

void OSCComponent::setupConnection()
{
    bool shouldConnect = enabled.boolValue() && WifiComponent::instance->state == WifiComponent::Connected;

    udpIsInit = false;
    if (shouldConnect)
    {
        // NDBG("Start OSC Receiver on " + String(OSC_LOCAL_PORT));
        udp.begin(OSC_LOCAL_PORT);
        udp.flush();
        isAlive.set(true);
        timeSinceLastReceivedPing = millis();

        if (MDNS.begin((DeviceID).c_str()))
        {
            MDNS.setInstanceName("Bentuino - " + DeviceID);
            NDBG("OSC Zeroconf started");
            MDNS.addService("osc", "udp", 9000);
            MDNS.addService("oscjson", "tcp", 80);
        }
        else
        {
            NDBG("Error setting up MDNS responder!");
        }

        udpIsInit = true;
    }
    else
    {
        // NDBG("Stopping Receiver");
        udp.flush();
        udp.stop();
        isAlive.set(false);

        MDNS.end();
    }
}

void OSCComponent::receiveOSC()
{
    if (!udpIsInit)
        return;

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

void OSCComponent::processMessage(OSCMessage &msg)
{

    if (msg.match("/yo"))
    {
        char hostData[32];
        msg.getString(0, hostData, 32);

        // NDBG("Yo received from : " + String(hostData));

        remoteHost.set(hostData);

        OSCMessage msg("/wassup");

        msg.add(WifiComponent::instance->getIP().c_str());
        msg.add(DeviceID.c_str());
        msg.add(ARDUINO_BOARD);

        sendMessage(msg);
    }
    else if (msg.match("/ping"))
    {
        // NDBG("Received ping");
        isAlive.set(true);
        pingEnabled = true;
        timeSinceLastReceivedPing = millis();

        if (msg.size() > 0)
        {
            char hostData[32];
            msg.getString(0, hostData, 32);
            remoteHost.set(hostData);
        }

        OSCMessage msg("/pong");
        msg.add(DeviceID.c_str());
        sendMessage(msg);
    }
    else
    {
        char addr[64];
        msg.getAddress(addr);
        String addrStr = String(addr).substring(1);
        addrStr.replace('/', '.');
        int tcIndex = addrStr.lastIndexOf('.');
        String tc = addrStr.substring(0, tcIndex); // component name
        String cmd = addrStr.substring(tcIndex + 1);

        const int numData = 10; // max 10-2 = 8 arguments
        var data[numData];
        data[0] = tc;
        data[1] = cmd;
        // NDBG(data[0].stringValue() + "." + data[1].stringValue());

        for (int i = 0; i < msg.size() && i < numData - 2; i++)
        {
            data[i + 2] = OSCArgumentToVar(msg, i);
        }

        sendEvent(MessageReceived, data, msg.size() + 2);
    }
}

void OSCComponent::sendMessage(OSCMessage &msg)
{
    if (!enabled.boolValue() || remoteHost.stringValue().length() == 0)
        return;

    char addr[32];
    msg.getAddress(addr);
    udp.beginPacket((char *)remoteHost.stringValue().c_str(), OSC_REMOTE_PORT);
    msg.send(udp);
    udp.endPacket();
    msg.empty();
}

void OSCComponent::sendMessage(String address)
{
    if (!enabled.boolValue() || remoteHost.stringValue().length() == 0)
        return;

    OSCMessage m(address.c_str());
    sendMessage(m);
}

void OSCComponent::sendMessage(const String &source, const String &command, var *data, int numData)
{
    if (!enabled.boolValue() || remoteHost.stringValue().length() == 0)
        return;

    OSCMessage msg = createMessage(source, command, data, numData, true);
    sendMessage(msg);
}

OSCMessage OSCComponent::createMessage(const String &source, const String &command, const var *data, int numData, bool addID)
{
    OSCMessage msg(("/" + source + "/" + command).c_str());
    if(addID) msg.add(DeviceID.c_str());
    for (int i = 0; i < numData; i++)
    {
        switch (data[i].type)
        {
        case 'f':
            msg.add(data[i].floatValue());
            break;

        case 'i':
            msg.add(data[i].intValue());
            break;

        case 's':
            msg.add(data[i].stringValue().c_str());
            break;

        case 'b':
            msg.add(data[i].boolValue());
            break;

        case 'T':
            msg.add(true);
            break;

        case 'F':
            msg.add(false);
            break;
        }
    }

    return msg;
}

var OSCComponent::OSCArgumentToVar(OSCMessage &m, int index)
{
    if (m.isString(index))
    {
        char str[32];
        m.getString(index, str);
        return str;
    }
    else if (m.isInt(index))
        return m.getInt(index);
    else if (m.isFloat(index))
        return m.getFloat(index);
    else if (m.isBoolean(index))
        return m.getBoolean(index);

    NDBG("OSC Type not supported !");
    return var(0);
}
