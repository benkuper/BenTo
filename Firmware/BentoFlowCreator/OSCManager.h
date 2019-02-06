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

    char remoteHost[32];
    const int remotePort = 10000;

    bool pingEnabled;
    long lastPingTime;
    long pingTime;
    bool isReadyToSend;

    OSCManager(WiFiUDP &udp): udp(udp)
    {
      sprintf(remoteHost, "192.168.0.13");

      addCallbackMessageReceived(&OSCManager::defaultCallback);

      isReadyToSend = false;
      pingEnabled = false;
      lastPingTime = 0;
      pingTime = 1000;
    }

    void init()
    {
#if SERIAL_DEBUG
      Serial.println("OCSManager init.");
#endif
    }

    void update()
    {
      if (pingEnabled) ping();
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

            msgIN.getString(0, remoteHost, 32);

#if SERIAL_DEBUG
            Serial.print("Got yo request from : ");
            Serial.println(remoteHost);
#endif

           OSCMessage msg("/wassup");
           msg.add(ipToString().c_str());
            msg.add(DeviceSettings::deviceID.c_str());
            msg.add(DeviceSettings::deviceType.c_str());
           sendMessage(msg);

          } else
          {
            onMessageReceived(msgIN);
          }
        } else
        {
#if SERIAL_DEBUG
          Serial.println("Msg got error");
#endif
        }
      }
    }


    void sendMessageTo(OSCMessage & msg, const char * host, int port)
    {
     
      if (!isReadyToSend) return;
      
#if SERIAL_DEBUG
      char addr[32];
      msg.getAddress(addr);
      Serial.println("Send OSC message " + String(addr)+", isReadyToSend ? "+String(isReadyToSend));
#endif

      udp.beginPacket(host, port);
      msg.send(udp);
      udp.endPacket();
      msg.empty();
    }

    void sendMessage(OSCMessage &msg)
    {
      sendMessageTo(msg, remoteHost, remotePort);
    }

    void ping()
    {
      if (millis() - lastPingTime < pingTime) return;

      OSCMessage msg("/ping");
      sendMessage(msg);

      lastPingTime = millis();
    }


    //Helpers
    String ipToString()
    {
      char myIpString[24];
      IPAddress myIp = WiFi.localIP();
      return String(myIp[0])+"."+String(myIp[1])+"."+String(myIp[2])+"."+String(myIp[3]);
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
