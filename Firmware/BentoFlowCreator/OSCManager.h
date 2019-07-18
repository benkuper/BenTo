#ifndef OSC_MANAGER_H
#define OSC_MANAGER_H

#include "DeviceSettings.h"
#include "WiFiManager.h"

#include <OSCMessage.h>
#include <OSCBoards.h>

class OSCManager
{
  public:
    WiFiUDP &udp;

    String remoteHost;
    const int remotePort = 10000;

    bool isReadyToSend;

#if USE_PING
    long lastPingTime;
    const long pingTime = 1000;
#endif

    OSCManager(WiFiUDP &udp): udp(udp)
    {
      addCallbackMessageReceived(&OSCManager::defaultCallback);
      isReadyToSend = false;
    }

    void init()
    {
      DBG("OCSManager init.");
      remoteHost = preferences.getString("remoteHost", "");
      lastPingTime = 0;
    }

    void update()
    {
#if USE_PING
      ping();
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
            preferences.putString("remoteHost", remoteHost);

            OSCMessage msg("/wassup");
            msg.add(ipToString().c_str());
            msg.add(DeviceSettings::deviceID.c_str());
            msg.add(DeviceSettings::deviceType.c_str());
            sendMessage(msg);

          }
          else if (msgIN.match("/ping"))
          {
            DBG("Got ping !");
            ping(true);
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

      if (!isReadyToSend) return;

      char addr[32];
      msg.getAddress(addr);
      DBG("Send OSC message " + String(addr) + ", isReadyToSend ? " + String(isReadyToSend));

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


    //Helpers
    String ipToString()
    {
      char myIpString[24];
      IPAddress myIp = WiFi.localIP();
      return String(myIp[0]) + "." + String(myIp[1]) + "." + String(myIp[2]) + "." + String(myIp[3]);
    }


    typedef void(*onEvent)(OSCMessage &);
    void (*onMessageReceived) (OSCMessage &);

    void addCallbackMessageReceived (onEvent func) {
      onMessageReceived = func;
    }

    static void defaultCallback(OSCMessage &msg)
    {
      //nothing
    }

};

#endif
