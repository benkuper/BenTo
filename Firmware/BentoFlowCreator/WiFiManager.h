#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "DeviceSettings.h"
#include <WiFi.h>

#if USE_BONJOUR
#else
#include <WiFiUDP.h>
#endif

#if USE_OSC
#include <OSCMessage.h>
#endif

#define CONNECT_TIMEOUT 5000
#define CONNECT_TRYTIME 500

class WiFiManager
{
  public:

    String ssid = "";
    String password = "";

    const int maxTries = 32;
    bool isConnected;
    bool isConnecting;

    bool autoAPMode;
    bool apMode;
    bool noWiFiMode;
    
    long timeAtStartConnect;
    long timeAtLastConnect;

    WiFiManager() {
      noWiFiMode = false;
      apMode = false;
      isConnected = false;
      isConnecting = false;
      addCallbackConnectionUpdate(&WiFiManager::connectionUpdateDefaultCallback);
    }

    void reset()
    {
      DBG("Reset Wifi !");
      init(true, false);
    }
    
    void init(bool silentMode = false, bool apModeIfFail = true)
    {
      autoAPMode = apModeIfFail;
      ssid = preferences.getString("wifiSSID", "jonglissimo");
      password = preferences.getString("wifiPassword", "lightpainting");

      DBG(String("WiFiManager init, connecting to " + ssid + " : " + password));

      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(), password.c_str());

      timeAtStartConnect = millis();
      timeAtLastConnect = millis();

      noWiFiMode = false;
      apMode = false;
      isConnected = false;
      isConnecting = true;
    }
    

    void update()
    {
      if (isConnected || noWiFiMode) return;
      if (!isConnecting) return;

      if (millis() > timeAtLastConnect + CONNECT_TRYTIME)
      {
        DBG(".");
        if (WiFi.status() == WL_CONNECTED)
        {
          DBG("WiFi Connected, local IP : " + String(WiFi.localIP()[0]) +
              "." + String(WiFi.localIP()[1]) +
              "." + String(WiFi.localIP()[2]) +
              "." + String(WiFi.localIP()[3]));

          setConnected(true);
          return;
        }
        
        timeAtLastConnect = millis();
      }

      //Timeout
      if (millis() > timeAtStartConnect + CONNECT_TIMEOUT)
      {
        if (autoAPMode) setupLocalWiFi();
      }
    }

    void setConnected(bool value)
    {
      isConnected = value;
      isConnecting = false;
      onConnectionUpdate();
    }

    void cancelConnection()
    {
      DBG("Turn OFF WiFi");
      noWiFiMode = true;
      WiFi.mode(WIFI_OFF);
      setConnected(false);
    }
    
    void setupLocalWiFi()
    {
      //uint8 mac[6];
      //WiFi.softAPmacAddress(mac);
      //String macString((const char *)mac);
      String wifiString(String("BenTo v4.2 ") +  settings.deviceID);

      DBG("Setting up AP WiFi : " + wifiString);
      WiFi.softAP(wifiString.c_str());
      DBG("WiFi AP created, IP address: " + String(WiFi.softAPIP()[0]) + "." + String( WiFi.softAPIP()[1]) + "." + String( WiFi.softAPIP()[2]) + "." + String( WiFi.softAPIP()[3]));

      apMode = true;
      setConnected(true);
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

          preferences.putString("wifiSSID", String(ssidData));
          preferences.putString("wifiPassword", String(passData));

        }
        return true;
      }

      return false;
    }
#endif


    //EVENTS

    typedef void(*onConnectionUpdateEvent)();
    void (*onConnectionUpdate) ();


    void addCallbackConnectionUpdate (onConnectionUpdateEvent func) {
      onConnectionUpdate = func;
    }



    static void connectionUpdateDefaultCallback()
    {
      //nothing
    }

};

#endif
