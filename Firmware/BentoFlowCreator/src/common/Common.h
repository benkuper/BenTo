//This file includes all common classes and definitions so it's the only one to include from other files

#pragma once

#include "Component.h"
#include "events/EventBroadcaster.h"
#include <Preferences.h>

enum ConnectionState { Off, Connecting, Connected, ConnectionError, Disabled, Hotspot, CONNECTION_STATES_MAX };
const String connectionStateNames[CONNECTION_STATES_MAX] {"Off", "Connecting", "Connected", "ConnectionError", "Disabled", "Hotspot"};