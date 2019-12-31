#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#if USE_OSC
#include <OSCMessage.h>
#endif


class DeviceSettings
{
public:
  static String deviceID;
  static uint32_t propID; //
  static String deviceType;


  DeviceSettings()
  {
  }

  void init()
  {
    uint64_t did = ESP.getChipId();
    deviceID = String((uint32_t)(did >> 32)) + String((uint32_t)(did & 0xffffffff));
    
    DBG("DeviceSettings init, device ID " + deviceID);
  }

#if USE_OSC
  bool handleMessage(OSCMessage &msg)
  {
    if (msg.match("/settings/propID"))
    {
      if (msg.size() > 0 && msg.isInt(0))
      {
        propID = msg.getInt(0);
        DBG("New PropID : " + String(propID));
      }
      else
      {
        DBG("Bad /setings/propID message format");
      }

      return true;
    }

    return false;
  }
#endif
};

String DeviceSettings::deviceID = "notset";
uint32_t DeviceSettings::propID = 0;
String DeviceSettings::deviceType = "Lightrix Hoop";

  
#endif //DEVICESETTINGS_H
