#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "DeviceSettings.h"
#include <WiFi.h>

#define USE_SERVER 0
#if USE_BONJOUR
#else
#include <WiFiUDP.h>
#endif

#if USE_OSC
#include <OSCMessage.h>
#endif

#if USE_SERVER
#include "WiFiSettingsServer.h";
#endif

class WiFiManager
{
  public:

    String ssid = "spacenet";
    String password = "jhfdb97531$$";

    const int maxTries = 32;

    WiFiUDP oscUDP;
    WiFiUDP streamingUDP;

    bool isConnected;
    bool apMode;
    bool turnOffWiFi;

#if USE_SERVER
    WiFiSettingsServer wServer;
#endif

    WiFiManager() {
      turnOffWiFi = false;
      apMode = false;
      isConnected = false;
      addCallbackConnectingUpdate(&WiFiManager::connectingUpdateDefaultCallback);
    }

    void init()
    {
#if SERIAL_DEBUG
      Serial.println("WiFiManager init.");
#endif

      // Connect to WiFi network


      WiFi.mode(WIFI_STA);
#if SERIAL_DEBUG
      WiFi.printDiag(Serial);
      Serial.println(String("WiFiManager connecting to " + ssid + " : " + password));
#endif;

      WiFi.begin(ssid.c_str(), password.c_str());

      int tryIndex = 0;
      bool success = true;

#if SERIAL_DEBUG
      Serial.print("Connecting : ");
#endif

      // Wait for connection
      while (WiFi.status() != WL_CONNECTED && !turnOffWiFi) {
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

      if (!success || turnOffWiFi)
      {
        if (turnOffWiFi)
        {
          WiFi.mode(WIFI_OFF);
#if SERIAL_DEBUG
          Serial.println("Turn OFF WiFi");
#endif
          return;
        } else
        {
          isConnected = false;
          setupLocalWiFi();
          apMode = true;
        }
      }



#if SERIAL_DEBUG
      if (apMode)
      {
        Serial.println("");
        Serial.print("WiFi AP created, IP address: ");
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


#if USE_BONJOUR
#endif

      isConnected = true;
      delay(500);
    }

    void cancelConnection()
    {
      turnOffWiFi = true;
    }

    void setupLocalWiFi()
    {
      //uint8 mac[6];
      //WiFi.softAPmacAddress(mac);
      //String macString((const char *)mac);
      String wifiString(String("BenTo v4.2 ") +  settings.deviceID);

#if SERIAL_DEBUG
      Serial.print("Setting up AP WiFi : ");
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
        if (wServer.ssid.length() > 0)
        {
          Serial.println("WiFi Manager saving config...");
          saveWiFiConfig(wServer.ssid.c_str(), wServer.pass.c_str());

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

          //saveWiFiConfig(ssidData, passData);
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
