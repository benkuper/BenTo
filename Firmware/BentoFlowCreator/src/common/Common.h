//This file includes all common classes and definitions so it's the only one to include from other files

#pragma once
#ifndef COMMON_H
#define COMMON_H

#include "config/Config.h"

#include "Component.h"
#include "events/EventBroadcaster.h"

#ifdef ESP32
#define USE_PREFERENCES
#include <Preferences.h>
#elif defined ESP8266
#define USE_SETTINGS_MANAGER
#include "settings/SettingsManager.h"
#endif

#include "Timer.h"

#define TSTART()         { tstart = micros(); }
#define TFINISH(s)       { tend = micros(); Serial.print(s " in "); Serial.print(tend-tstart); Serial.println(" us"); }


enum ConnectionState { Off, Connecting, Connected, ConnectionError, Disabled, Hotspot, PingAlive, PingDead, CONNECTION_STATES_MAX };
const String connectionStateNames[CONNECTION_STATES_MAX] {"Off", "Connecting", "Connected", "ConnectionError", "Disabled", "Hotspot", "PingAlive","PingDead" };

String getDeviceID();

#endif