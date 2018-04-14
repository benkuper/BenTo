#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "DeviceSettings.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "FS.h"

#define USE_BONJOUR 0
#define USE_SERVER 0

#if USE_BONJOUR
#include <ESP8266mDNS.h>
MDNSResponder mdns;
#else
#include <WiFiUDP.h>
#endif

#if USE_OSC
#include <OSCMessage.h>
#endif

#if USE_SERVER
#include "WifiSettingsServer.h";
#endif

class WifiManager
{
  public:

    String ssid = "flowspace";
    String password = "flowarts";

    const int maxTries = 50;

    WiFiUDP oscUDP;
    WiFiUDP streamingUDP;

    bool isConnected;
    bool apMode;
    bool turnOffWifi;

#if USE_SERVER
    WifiSettingsServer wServer;
#endif

    WifiManager() {
      turnOffWifi = false;
      apMode = false;
      isConnected = false;
      addCallbackConnectingUpdate(&WifiManager::connectingUpdateDefaultCallback);
    }

    void init()
    {
#if SERIAL_DEBUG
      Serial.println("WifiManager init.");
#endif

      // Connect to WiFi network
      #if SERIAL_DEBUG
      Serial.println(String("WifiManager connecting to "+ssid+" : "+password));
      #endif

      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(),password.c_str());

      int tryIndex = 0;
      bool success = true;

#if SERIAL_DEBUG
      Serial.print("Connecting : ");
#endif

      // Wait for connection
      while (WiFi.status() != WL_CONNECTED && !turnOffWifi) {
        delay(500);

#if SERIAL_DEBUG
        Serial.print("*");
#endif

        if (tryIndex >= maxTries)
        {
          success = false;
          break;
        }
        onConnectingUpdate(tryIndex);
        tryIndex++;
      }

#if SERIAL_DEBUG
      if (!success) Serial.print("Failed");
      Serial.println();
#endif

      if (!success || turnOffWifi)
      {
        if(turnOffWifi)
        {
          WiFi.mode(WIFI_OFF);
          #if SERIAL_DEBUG
          Serial.println("Turn OFF Wifi");
          #endif
          return;
        }else
        {
          isConnected = false;
          setupLocalWifi();
          apMode = true;
        }
      }
      


#if SERIAL_DEBUG
      if (apMode)
      {
        Serial.println("");
        Serial.print("Wifi AP created, IP address: ");
        Serial.println(WiFi.softAPIP());
      } else
      {
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
      }

#endif
      delay(500);

#if SERIAL_DEBUG
      Serial.print("Setting up UDP...");
#endif

      oscUDP.begin(9000);
      streamingUDP.begin(8888);

#if SERIAL_DEBUG
      Serial.println("OK");
#endif

/*
#if USE_BONJOUR
  #if SERIAL_DEBUG
        Serial.print("Registering mDNS...");
        Serial.print("...");
  #endif

      if (!mdns.begin("bento", apMode ? WiFi.softAPIP() : WiFi.localIP())) {
#if SERIAL_DEBUG
        Serial.println("Error setting up MDNS responder!");
#endif
      } else
      {
#if SERIAL_DEBUG
        Serial.print("OK : ");
        Serial.println("Registering Bonjour service...");
#endif

        mdns.addService("ledcontrol", "udp", 8888);
#endif
      }
      */
      isConnected = true;
      delay(500);
    }

    void cancelConnection()
    {
      turnOffWifi = true;
    }

    void setupLocalWifi()
    {
      //uint8 mac[6];
      //WiFi.softAPmacAddress(mac);
      //String macString((const char *)mac);
      String wifiString(String("BenTo v4.2 ") +  ESP.getChipId());

#if SERIAL_DEBUG
      Serial.print("Setting up AP Wifi : ");
      Serial.println(wifiString);
#endif


      WiFi.softAP(wifiString.c_str());

#if USE_SERVER
      wServer.init();
#endif //end USE_SERVER
    }

    void update()
    {
#if USE_SERVER
      if (apMode) 
      {
        wServer.update();
        if(wServer.ssid.length() > 0)
        {
          Serial.println("Wifi Manager saving config...");
          saveWifiConfig(wServer.ssid.c_str(),wServer.pass.c_str());
          
        }
      }
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

          //saveWifiConfig(ssidData, passData);
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


