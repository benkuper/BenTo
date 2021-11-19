#include "UnityIncludes.h"

#define UNITY_COMPILE

//Libraries

#include "Component/components/ledstrip/lib/NeoPixel/esp.c"
#include "Component/components/ledstrip/lib/NeoPixel/Adafruit_NeoPixel.cpp"


#include "Component/components/imu/lib/Adafruit_Sensor.cpp"
#include "Component/components/imu/lib/Adafruit_BNO055.cpp"


// Firmware

#include "Component/ComponentEvent.cpp"
#include "Component/Component.cpp"

#include "Component/components/settings/SettingsComponent.cpp"
#include "Component/components/wifi/WifiComponent.cpp"
#include "Component/components/battery/BatteryComponent.cpp"
#include "Component/components/communication/CommunicationComponent.cpp"
#include "Component/components/communication/osc/OSCComponent.cpp"
#include "Component/components/communication/serial/SerialComponent.cpp"
#include "Component/components/server/WebServerComponent.cpp"

#include "Component/components/sequence/SequenceComponent.cpp"
#include "Component/components/files/FilesComponent.cpp"

#include "Component/components/ledstrip/Layer/LedStripLayer.cpp"
#include "Component/components/ledstrip/Layer/layers/stream/LedStripStreamLayer.cpp"
#include "Component/components/ledstrip/Layer/layers/bake/LedStripBakeLayer.cpp"
#include "Component/components/ledstrip/Layer/layers/script/LedStripScriptLayer.cpp"
#include "Component/components/ledstrip/Layer/layers/system/LedStripSystemLayer.cpp"

#include "RootComponent.cpp"