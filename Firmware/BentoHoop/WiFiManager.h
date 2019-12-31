#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "DeviceSettings.h"

#include <ESP8266WiFi.h>       // Standard WiFi Library
#include <WiFiUDP.h>           // UDP Protocol Library

#if USE_OSC
#include <OSCMessage.h>
#endif


class WiFiManager
{
  public:

    String ssid = "";
    String password = "";

    const int maxTries = 32;

    WiFiUDP oscUDP;
    WiFiUDP streamingUDP;

    bool isConnected;
    bool apMode;
    bool turnOffWiFi;


    WiFiManager() {
      turnOffWiFi = false;
      apMode = false;
      isConnected = false;
      addCallbackConnectingUpdate(&WiFiManager::connectingUpdateDefaultCallback);
    }

    void init(bool silentMode = false, bool apModeIfFail = true)
    {
      DBG("WiFiManager init.");
  #if USE_PREFERENCES
      ssid = preferences.getString("wifiSSID", "flowspace");
      password = preferences.getString("wifiPassword", "flowarts");
  #else
      ssid = "flowspace";
      password = "flowarts";
  #endif
  
      DBG(String("WiFiManager connecting to " + ssid + " : " + password));

      WiFi.begin((char *)ssid.c_str(), (char *)password.c_str());

      int tryIndex = 0;
      bool success = true;

      DBG("Connecting : ");

      // Wait for connection
      while (WiFi.status() != WL_CONNECTED && !turnOffWiFi) {
        delay(500);

        DBG("*");

        if (tryIndex >= maxTries)
        {
          success = false;
          break;
        }
        if (!silentMode) onConnectingUpdate(tryIndex);
        tryIndex++;
      }


      if (!success)
      {
        DBG("Failed");
      }

      DBG("");

      if (!success || turnOffWiFi)
      {
        if (turnOffWiFi)
        {
          WiFi.mode(WIFI_OFF);
          DBG("Turn OFF WiFi");
          return;
        } else if (apModeIfFail)
        {
          isConnected = false;
          setupLocalWiFi();
          apMode = true;
        } else
        {
          isConnected = false;
          return;
        }
      }

      if (apMode)
      {
        DBG("WiFi AP created, IP address: " + String(WiFi.softAPIP()[0]) + "." + String( WiFi.softAPIP()[1]) + "." + String( WiFi.softAPIP()[2]) + "." + String( WiFi.softAPIP()[3]));
      } else
      {
        DBG("Connected to " + String(ssid) + ", IP address: " + String(WiFi.localIP()[0]) + "." + String( WiFi.localIP()[1]) + "." + String( WiFi.localIP()[2]) + "." + String( WiFi.localIP()[3]));
      }

      delay(500);

      //DBG("Disabling wifi sleep mode");
      //WiFi.setSleep(false);

      DBG("Starting Connections");
      startConnections();

      isConnected = true;
      delay(500);
    }

    void cancelConnection()
    {
      turnOffWiFi = true;
    }

    void reset()
    {
      DBG("Reset Wifi !");
      init(true, false);
    }

    void setupLocalWiFi()
    {
      //uint8 mac[6];
      //WiFi.softAPmacAddress(mac);
      //String macString((const char *)mac);
      String wifiString(String("BenTo v4.2 ") +  settings.deviceID);

      DBG("Setting up AP WiFi : " + wifiString);
      WiFi.softAP(wifiString.c_str());
    }


    void stopConnections()
    {
#if USE_OSC
      oscUDP.flush();
      oscUDP.stop();
      DBG("OSC UDP is stopped.");
#endif

#if USE_LEDSTRIP
      streamingUDP.flush();
      streamingUDP.stop();
      DBG("Streaming UPD is stopped.");
#endif
    }

    void startConnections()
    {
#if USE_OSC
      oscUDP.begin(9000);
      oscUDP.flush();
      DBG("OSC UDP is init.");
#endif

#if USE_LEDSTRIP
      streamingUDP.begin(8888);
      streamingUDP.flush();
      DBG("Streaming UPD is init.");
#endif
    }

#if USE_OSC
    boolean handleMessage(OSCMessage &msg)
    {

      if (msg.match("/wifiConfig"))
      {
        if (msg.size() >= 2)
        {
          char ssidData[32];
          char passData[32];
          msg.getString(0, ssidData, msg.getDataLength(0));
          msg.getString(1, passData, msg.getDataLength(1));
          
          #if USE_PREFERENCES
          preferences.putString("wifiSSID", String(ssidData));
          preferences.putString("wifiPassword", String(passData));
          #endif

        }
        return true;
      }

      return false;
    }
#endif


    //EVENTS

    typedef void(*onConnectingUpdateEvent)(int);
    void (*onConnectingUpdate) (int);


    void addCallbackConnectingUpdate (onConnectingUpdateEvent func) {
      onConnectingUpdate = func;
    }



    static void connectingUpdateDefaultCallback(int curTry)
    {
      //nothing
    }

};

#endif
