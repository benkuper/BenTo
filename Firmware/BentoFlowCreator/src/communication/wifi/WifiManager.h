#pragma once

#include "../../common/Common.h"
#include <WiFi.h>


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

  Preferences prefs;

  const long timeBetweenTries = 500; //ms
  const long connectionTimeout = 5000; //ms
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