#ifndef OSC_MANAGER_H
#define OSC_MANAGER_H

#include "DeviceSettings.h"
#include "WiFiManager.h"

#include <OSCMessage.h>
#include <OSCBoards.h>

#define PING_RECEIVE_TIMEOUT 8000

class OSCManager
{
  public:
    WiFiUDP &udp;

    String remoteHost;
    const int remotePort = 10000;

    bool isReadyToSend;
    bool isConnected;

#if USE_PING
    long lastPingTime;
    const long pingTime = 1000;
    long timeSinceLastReceivedPing;
    bool pongEnabled = true;
#endif

    OSCManager(WiFiUDP &udp): udp(udp)
    {
      addCallbackMessageReceived(&OSCManager::defaultCallback);
      addCallbackConnectionChanged(&OSCManager::defaultConnectionCallback);
      isReadyToSend = false;
    }

    void init()
    {
      DBG("OCSManager init.");
      #if USE_PREFERENCES
remoteHost = preferences.getString("remoteHost", "");
      #else
      remoteHost = "";
      #endif
      
      #if USE_PING
      lastPingTime = 0;
      timeSinceLastReceivedPing = millis();
      pongEnabled = true;
      #endif
      
      isConnected = true;
    }

    void update()
    {
#if USE_PING
      ping();
      if(isConnected && pongEnabled && millis() > timeSinceLastReceivedPing + PING_RECEIVE_TIMEOUT)
      {
        DBG("MAY BE DISCONNECTED ?");
        pongEnabled = false;
        
        setConnected(false);
      }
#endif

      receiveOSC();
    }

    void receiveOSC()
    {
      OSCMessage msgIN;
      int size;

      if ( (size = udp.parsePacket()) > 0)
      {
        while (size--)
        {
          msgIN.fill(udp.read());
        }

        if (!msgIN.hasError())
        {
          if (msgIN.match("/yo"))
          {

            char hostData[32];
            msgIN.getString(0, hostData, 32);
            remoteHost = String(hostData);
            DBG("Got yo request from : " + remoteHost);
            
            #if USE_PREFERENCES
            preferences.putString("remoteHost", remoteHost);
            #endif

            OSCMessage msg("/wassup");
            DBG("Send Wassup with type :"+String(DeviceSettings::deviceType));
            msg.add(ipToString().c_str());
            msg.add(DeviceSettings::deviceID.c_str());
            msg.add(DeviceSettings::deviceType.c_str());
            sendMessage(msg);

          }
          else if (msgIN.match("/ping"))
          {
            DBG("Got ping !");
            ping(true);
            pongEnabled = true;
            setConnected(true);
            timeSinceLastReceivedPing = millis();
          }
          else
          {
            onMessageReceived(msgIN);
          }
        } else
        {
          DBG("Msg got error");
        }
      }
    }


    void sendMessageTo(OSCMessage & msg, const char * host, int port)
    {
      
      char addr[32];
      msg.getAddress(addr);
      //DBG("Send OSC message " + String(addr) + ", isReadyToSend ? " + String(isReadyToSend));
      
      if (!isReadyToSend) return;
      
      udp.beginPacket(host, port);
      msg.send(udp);
      udp.endPacket();
      msg.empty();
    }

    void sendMessage(OSCMessage &msg)
    {
      if (remoteHost.length() == 0)
      {
        DBG("Remote host address is empty, not sending !");
        return;
      }
      
      //DBG("Send Message to  "+remoteHost+":"+String(remotePort));
     sendMessageTo(msg, remoteHost.c_str(), remotePort);
    }

    void ping(bool isPong = false)
    {
      if (!isPong && (millis() - lastPingTime < pingTime)) return;
      
      OSCMessage msg(isPong?"/pong":"/ping");
      msg.add(DeviceSettings::deviceID.c_str());
      sendMessage(msg);

      if(!isPong) lastPingTime = millis();
    }

    void setConnected(bool value)
    {
      if(isConnected == value) return;
      isConnected = value;
      onConnectionChanged(isConnected);
    }

    //Helpers
    String ipToString()
    {
      char myIpString[24];
      IPAddress myIp = WiFi.localIP();
      return String(myIp[0]) + "." + String(myIp[1]) + "." + String(myIp[2]) + "." + String(myIp[3]);
    }


    typedef void(*onEvent)(OSCMessage &);
    void (*onMessageReceived) (OSCMessage &);

    typedef void (*onConnectionEvent)(bool);
    void (*onConnectionChanged) (bool);

    void addCallbackMessageReceived (onEvent func) {
      onMessageReceived = func;
    }

    void addCallbackConnectionChanged (onConnectionEvent func) {
      onConnectionChanged = func;
    }

    static void defaultCallback(OSCMessage &msg)
    {
      //nothing
    }

    static void defaultConnectionCallback(bool isConnected)
    {
      //nothing
    }
};

#endif
