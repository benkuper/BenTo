
#include "CommunicationManager.h"

CommunicationManager::CommunicationManager() : Component("comm")
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
            float f = s.toFloat();
            int i = s.toInt();
            if (f != 0 || s.indexOf('0') == 0)
            {
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
    NDBG("Connection event update "+connectionStateNames[wifiManager.state]);
    EventBroadcaster<ConnectionEvent>::sendEvent(ConnectionEvent(wifiManager.state, 
                              wifiManager.name, 
                              (wifiManager.state == Connected || wifiManager.state == Hotspot)?wifiManager.getIP():""
                             )
             );
}

void CommunicationManager::oscMessageEvent(const OSCEvent &e)
{
    char addr[32];
    e.msg->getAddress(addr, 1); //remove the first slash
    String tc(addr);
    int tcIndex = tc.indexOf('/');

    int numData = e.msg->size();
    var *data = (var *)malloc(numData * sizeof(var));

    for (int i = 0; i < e.msg->size(); i++)
    {
        switch (e.msg->getType(i))
        {
        case 'i':
            data[i].value.i = e.msg->getInt(i);
            data[i].type = 'i';
            break;
        case 'f':
            data[i].value.f = e.msg->getFloat(i);
            data[i].type = 'f';
            break;
        case 's':
            e.msg->getString(i, data[i].value.s);
            data[i].type = 's';
            break;
        }
    }

    EventBroadcaster<CommunicationEvent>::sendEvent(CommunicationEvent(CommunicationEvent::MessageReceived, oscManager.name, tc.substring(0, tcIndex), tc.substring(tcIndex + 1), data, numData));
    free(data);
}