
#include "CommunicationManager.h"

CommunicationManager::CommunicationManager() :
    Component("comm")
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

void CommunicationManager::update() {
    serialManager.update();
    wifiManager.update();
    oscManager.update();
}


void CommunicationManager::serialMessageEvent(SerialEvent e) 
{
    NDBG("Serial Event "+String(e.type)+", target : "+e.target);
    switch(e.type)
    {
        case SerialEvent::MessageReceived:
         sendEvent(CommunicationEvent(CommunicationEvent::MessageReceived, e.target, e.data));
         break;
    }
}

void CommunicationManager::wifiConnectionEvent(WifiManagerEvent e) 
{
    //NDBG("Wifi connection Event "+wifiManager.stateNames[wifiManager.state]);
    if(wifiManager.state == WifiManager::Connected) NDBG("Connected with IP "+wifiManager.getIP());
}


void CommunicationManager::oscMessageEvent(OSCEvent e) 
{
    char addr[32];
    e.message.getAddress(addr, 0);
    NDBG("OSC Message received "+String(addr));
}