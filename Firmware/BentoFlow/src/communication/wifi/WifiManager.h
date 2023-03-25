#pragma once

#include "../../common/Common.h"

#ifdef ESP32
#include <WiFi.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef USE_ETHERNET
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12
#include <ETH.h>
#define NET ETH
#else
#define NET WiFi
#endif

#ifndef HOST_NAME
#define HOST_NAME "bento"
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
  const long connectionTimeout = 10000; //ms
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


  void WiFiEvent(system_event_id_t event, system_event_info_t info);

  void saveWifiConfig(String ssid, String pass);
  String getIP();

  bool handleCommand(String command, var * data, int numData) override;
};