//This file includes all common classes and definitions so it's the only one to include from other files

#pragma once

#include "config/Config.h"

#include "Component.h"
#include "events/EventBroadcaster.h"
#include <Preferences.h>
#include "Timer.h"

enum ConnectionState { Off, Connecting, Connected, ConnectionError, Disabled, Hotspot, PingAlive, PingDead, CONNECTION_STATES_MAX };
const String connectionStateNames[CONNECTION_STATES_MAX] {"Off", "Connecting", "Connected", "ConnectionError", "Disabled", "Hotspot", "PingAlive","PingDead" };

String getDeviceID();