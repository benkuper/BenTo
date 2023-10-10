#pragma once

#include "Arduino.h"
#include <Wire.h>

// Declarations
#define ARDUINOJSON_USE_LONG_LONG 0
#define ARDUINOJSON_USE_DOUBLE 0

// Libraries
 #include <Adafruit_NeoPixel.h>
 #include <Adafruit_DotStar.h>
 #include <utility/vector.h>
 #include <utility/matrix.h>
 #include <utility/quaternion.h>
 #include <Adafruit_Sensor.h>
 #include <Adafruit_BNO055.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <OSCMessage.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <arduino-timer.h>
#include <SimplexNoise.h>
#include <ESP32Servo.h>
#include <ArtnetWifi.h>
#include <FastAccelStepper.h>

#include "../lib/SD/src/SD.h" //really weird

#include <wasm3.h>
#include <m3_env.h>

// Firmware

#include "Common/Helpers.h"
#include "Common/BoardDefines.h"
#include "Common/var.h"
#include "Common/color.h"
#include "Common/EventBroadcaster.h"
#include "Common/Parameter.h"
#include "Common/Settings.h"
#include "Common/ParsingHelper.h"

#include "Common/script/Script.h"
#include "Common/script/wasmFunctions.h"

#include "Component/ComponentEvent.h"
#include "Component/Component.h"

#include "Component/components/wifi/WifiComponent.h"
#include "Component/components/communication/osc/OSCComponent.h"
#include "Component/components/communication/serial/SerialComponent.h"
#include "Component/components/communication/CommunicationComponent.h"
#include "Component/components/files/FilesComponent.h"
#include "Component/components/server/WebServerComponent.h"

#include "Component/components/script/ScriptComponent.h"

#include "Component/components/io/IOComponent.h"
#include "Component/components/io/button/ButtonComponent.h"
#include "Component/components/battery/BatteryComponent.h"
#include "Component/components/imu/IMUComponent.h"

#include "Component/components/ledstrip/Layer/LedStripLayer.h"
#include "Component/components/ledstrip/Layer/layers/stream/LedStripStreamLayer.h"
#include "Component/components/ledstrip/Layer/layers/bake/LedStripBakeLayer.h"
#include "Component/components/ledstrip/Layer/layers/script/LedStripScriptLayer.h"
#include "Component/components/ledstrip/Layer/layers/system/LedStripSystemLayer.h"
#include "Component/components/ledstrip/LedStripComponent.h"

#include "Component/components/sequence/SequenceComponent.h"

#include "Component/components/servo/ServoComponent.h"
#include "Component/components/stepper/StepperComponent.h"


#include "Component/components/behaviour/BehaviourComponent.h"
#include "Component/components/behaviour/BehaviourManager.h"

#include "Component/components/dummy/DummyComponent.h"


#include "RootComponent.h"