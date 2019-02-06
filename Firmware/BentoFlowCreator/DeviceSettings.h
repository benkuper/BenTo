#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#if USE_OSC
#include <OSCMessage.h>
#endif

class DeviceSettings
{
  public :

    static String deviceID;
    static uint32_t propID; //
    static String deviceType;
    
    DeviceSettings()
    {

    }

    void init()
    {
      uint64_t did = ESP.getEfuseMac();
      deviceID = String((uint32_t)(did >> 32)) + String((uint32_t)(did & 0xffffffff));

#if SERIAL_DEBUG
      Serial.println("DeviceSettings init.");
      Serial.print("Device ID : ");
      Serial.println(deviceID);
#endif
    }

#if USE_OSC
    bool handleMessage(OSCMessage &msg)
    {
      if (msg.match("/settings/propID"))
      {
        if (msg.size() > 0 && msg.isInt(0))
        {
          propID = msg.getInt(0);
#if SERIAL_DEBUG
          Serial.println("New PropID : " + String(propID));
#endif
        } else
        {
#if SERIAL_DEBUG
          Serial.println("Bad /setings/propID message format");
#endif
        }

        return true;
      }

      return false;

    }
#endif
};

String DeviceSettings::deviceID = "notset";
uint32_t DeviceSettings::propID = 0;
String DeviceSettings::deviceType = "Flowtoys Creator Club";

#endif //DEVICESETTINGS_H
