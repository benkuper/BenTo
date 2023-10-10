#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <String>

// Declarations
#define ARDUINOJSON_USE_LONG_LONG 0
#define ARDUINOJSON_USE_DOUBLE 0

#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <arduino-timer.h>

// Firmware
#include "Common/Helpers.h"
#include "Common/BoardDefines.h"
#include "Common/var.h"
#include "Common/color.h"
#include "Common/EventBroadcaster.h"
#include "Common/Parameter.h"
#include "Common/Settings.h"
#include "Common/ParsingHelper.h"


#include "Component/ComponentEvent.h"
#include "Component/Component.h"

#ifdef USE_FILES
#include "../lib/SD/src/SD.h" //really weird
#include "Component/components/files/FilesComponent.h"
#endif

#ifdef USE_WIFI
#include "Component/components/wifi/WifiComponent.h"
#endif

#ifdef USE_OSC
#include <OSCMessage.h>
#include <ESPmDNS.h>
#include "Component/components/communication/osc/OSCComponent.h"
#endif

#ifdef USE_SERIAL
#include "Component/components/communication/serial/SerialComponent.h"
#endif

#include "Component/components/communication/CommunicationComponent.h"

#ifdef USE_SERVER
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Component/components/server/WebServerComponent.h"
#endif

#ifdef USE_SCRIPT
#include <wasm3.h>
#include <m3_env.h>
#include <SimplexNoise.h>
#include "Common/script/Script.h"
#include "Common/script/wasmFunctions.h"
#include "Component/components/script/ScriptComponent.h"
#endif

#ifdef USE_IO
#include "Component/components/io/IOComponent.h"
#ifdef USE_BUTTON
#include "Component/components/io/button/ButtonComponent.h"
#endif // BUTTON
#endif // IO

#ifdef USE_BATTERY
#include "Component/components/battery/BatteryComponent.h"
#endif

#ifdef USE_IMU
#include <utility/vector.h>
#include <utility/matrix.h>
#include <utility/quaternion.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "Component/components/imu/IMUComponent.h"
#endif

#ifdef USE_LEDSTRIP
#include <Adafruit_NeoPixel.h>
#include <Adafruit_DotStar.h>

#ifdef USE_ARTNET
#include <ArtnetWifi.h>
#endif //ARTNET

#include "Component/components/ledstrip/Layer/LedStripLayer.h"
#ifdef USE_STREAM
#include "Component/components/ledstrip/Layer/layers/stream/LedStripStreamLayer.h"
#endif
#include "Component/components/ledstrip/Layer/layers/bake/LedStripBakeLayer.h"
#include "Component/components/ledstrip/Layer/layers/script/LedStripScriptLayer.h"
#include "Component/components/ledstrip/Layer/layers/system/LedStripSystemLayer.h"
#include "Component/components/ledstrip/LedStripComponent.h"
#endif //LEDSTRIP

#ifdef USE_SEQUENCE
#include "Component/components/sequence/SequenceComponent.h"
#endif

#ifdef USE_SERVO
#include <ESP32Servo.h>
#include "Component/components/servo/ServoComponent.h"
#endif

#ifdef USE_STEPPER
#include <FastAccelStepper.h>
#include "Component/components/stepper/StepperComponent.h"
#endif

#ifdef USE_BEHAVIOUR
#include "Component/components/behaviour/BehaviourComponent.h"
#endif

#ifdef USE_DUMMY
#include "Component/components/dummy/DummyComponent.h"
#endif

#include "RootComponent.h"