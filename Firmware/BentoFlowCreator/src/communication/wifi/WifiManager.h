#pragma once

#include "../../common/Common.h"


#include "Preferences.h"
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

  enum WifiState { Off, Disconnected, Connecting, Connected, Hotspot, STATES_MAX };
  const String stateNames[STATES_MAX] {"Off", "Disconnected", "Connecting", "Connected", "Hotspot"};
  WifiState state;

  Preferences prefs;

  const long timeBetweenTries = 500; //ms
  const long connectionTimeout = 5000; //ms
  long timeAtConnect;
  long lastConnectTime;

  void setState(WifiState s);

  void init();

  void connect();
  void disconnect();
  void turnOff();

  void update();

  String getIP();
};