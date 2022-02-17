#pragma once

#include "../../common/Common.h"

#ifdef ESP32
#include <WiFi.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#endif

class WifiManagerEvent
{
public:
  WifiManagerEvent() {}
};
  
class WifiManager : 
  public Component,
  public EventBroadcaster<WifiManagerEvent>
{
public:
  
  WifiManager();
  ~WifiManager() {}

  ConnectionState state;

#ifdef USE_PREFERENCES
  Preferences prefs;
#elif defined USE_SETTINGS_MANAGER
  SettingsManager prefs;
#endif

  const long timeBetweenTries = 500; //ms
#ifdef ESP32
  const long connectionTimeout = 5000; //ms
#else
  const long connectionTimeout = 10000; //ms
#endif

  long timeAtConnect;
  long lastConnectTime;

  void setState(ConnectionState s);

  void init();

  void connect();
  void disconnect();
  void disable();

  void update();

  void saveWifiConfig(String ssid, String pass);
  String getIP();

  bool handleCommand(String command, var * data, int numData) override;
};