#pragma once


#include "Arduino.h"
#include <Wire.h>

//Libraries
#include <Adafruit_NeoPixel.h>
#include <utility/vector.h>
#include <utility/matrix.h>
#include <utility/quaternion.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <ArduinoJson.h>

// Firmware

#include "Common/BoardDefines.h"
#include "Common/var.h"
#include "Common/color.h"
#include "Common/Helpers.h"
#include "Common/EventBroadcaster.h"


#include "Controllable/Controllable.h"
#include "Controllable/Parameter.h"
#include "Component/ComponentEvent.h"
#include "Component/Component.h"

#include "Component/components/settings/SettingsComponent.h"

#include "Component/components/wifi/WifiComponent.h"
#include "Component/components/battery/BatteryComponent.h"
#include "Component/components/communication/osc/OSCComponent.h"
#include "Component/components/communication/serial/SerialComponent.h"
#include "Component/components/communication/CommunicationComponent.h"
#include "Component/components/files/FilesComponent.h"
#include "Component/components/imu/IMUComponent.h"
#include "Component/components/io/IOComponent.h"

#include "Component/components/ledstrip/Layer/LedStripLayer.h"
#include "Component/components/ledstrip/Layer/layers/stream/LedStripStreamLayer.h"
#include "Component/components/ledstrip/Layer/layers/bake/LedStripBakeLayer.h"
#include "Component/components/ledstrip/Layer/layers/script/LedStripScriptLayer.h"
#include "Component/components/ledstrip/Layer/layers/system/LedStripSystemLayer.h"

#include "Component/components/ledstrip/LedStripComponent.h"

#include "Component/components/sequence/SequenceComponent.h"
#include "Component/components/server/WebServerComponent.h"
#include "Component/components/servo/ServoComponent.h"
#include "Component/components/stepper/StepperComponent.h"

#include "RootComponent.h"