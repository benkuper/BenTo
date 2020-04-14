
#include "CommunicationManager.h"

CommunicationManager::CommunicationManager() : Component("comm"),
                                               oscManager(&wifiManager)
{
}

void CommunicationManager::init()
{
    serialManager.addListener(std::bind(&CommunicationManager::serialMessageEvent, this, std::placeholders::_1));
    serialManager.init();

    wifiManager.addListener(std::bind(&CommunicationManager::wifiConnectionEvent, this, std::placeholders::_1));
    wifiManager.init();

    oscManager.addListener(std::bind(&CommunicationManager::oscMessageEvent, this, std::placeholders::_1));
    oscManager.init();
}

void CommunicationManager::update()
{
    serialManager.update();
    wifiManager.update();
    oscManager.update();
}

void CommunicationManager::serialMessageEvent(const SerialEvent &e)
{
    switch (e.type)
    {
    case SerialEvent::MessageReceived:
    {
        const int maxData = 16;
        var *data = (var *)malloc(maxData * sizeof(var)); //max 16 arguments
        int index = 0;

        //COUNT
        char *pch = strtok((char *)e.data.c_str(), ",");
        while (pch != NULL && index < maxData)
        {
            String s = String(pch);

            bool isNumber = true;
            for (byte i = 0; i < s.length(); i++)
            {
                char c = s.charAt(i);
                if (c != '.' && c != '-' && c != '+' && !isDigit(c))
                {
                    isNumber = false;
                    break;
                }
            }

            if (isNumber)
            {
                float f = s.toFloat();
                int i = s.toInt();

                if (f == i && s.indexOf('.') == -1)
                {
                    data[index].value.i = i;
                    data[index].type = 'i';
                }
                else
                {
                    data[index].value.f = f;
                    data[index].type = 'f';
                }
            }
            else
            {
                data[index].value.s = pch;
                data[index].type = 's';
            }

            pch = strtok(NULL, ",");
            index++;
        }

        EventBroadcaster<CommunicationEvent>::sendEvent(CommunicationEvent(CommunicationEvent::MessageReceived, serialManager.name, e.target, e.command, data, index));
        free(data);
    }
    break;
    }
}

void CommunicationManager::wifiConnectionEvent(const WifiManagerEvent &e)
{
    NDBG("Connection event update " + connectionStateNames[wifiManager.state]);
    EventBroadcaster<ConnectionEvent>::sendEvent(ConnectionEvent(wifiManager.state,
                                                                 wifiManager.name,
                                                                 (wifiManager.state == Connected || wifiManager.state == Hotspot) ? wifiManager.getIP() : ""));
}

void CommunicationManager::oscMessageEvent(const OSCEvent &e)
{
    if (e.type == OSCEvent::AliveChanged)
    {
        EventBroadcaster<ConnectionEvent>::sendEvent(ConnectionEvent(oscManager.isAlive ? ConnectionState::PingAlive : ConnectionState::PingDead, oscManager.name));
    }
    if (e.type == OSCEvent::MessageReceived)
    {
        char addr[32];
        e.msg->getAddress(addr, 1); //remove the first slash
        String tc(addr);
        int tcIndex = tc.indexOf('/');

        int numData = e.msg->size();
        var *data = (var *)malloc(numData * sizeof(var));
        int numUsedData = 0;

        char tmpStr[32][32]; //contains potential strings

        for (int i = 0; i < e.msg->size(); i++)
        {
            switch (e.msg->getType(i))
            {
            case 'i':
                data[i].value.i = e.msg->getInt(i);
                data[i].type = 'i';
                numUsedData++;
                break;
            case 'f':
                data[i].value.f = e.msg->getFloat(i);
                data[i].type = 'f';
                numUsedData++;
                break;
            case 's':
                e.msg->getString(i, tmpStr[i]);
                data[i].value.s = tmpStr[i];
                data[i].type = 's';
                numUsedData++;
                break;

            default:
                break;
            }
        }

        EventBroadcaster<CommunicationEvent>::sendEvent(CommunicationEvent(CommunicationEvent::MessageReceived, oscManager.name, tc.substring(0, tcIndex), tc.substring(tcIndex + 1), data, numUsedData));

        free(data);
    }
}

void CommunicationManager::sendMessage(String source, String command, var *data, int numData)
{
    serialManager.sendMessage(source, command, data, numData);
    oscManager.sendMessage(source, command, data, numData);
}