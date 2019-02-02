#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#if USE_OSC
#include <OSCMessage.h>
#endif

class DeviceSettings
{
  public :

    static uint32_t deviceID;
    static uint32_t propID; //

    DeviceSettings()
    {

    }

    void init()
    {
      deviceID = ESP.getEfuseMac();

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

uint32_t DeviceSettings::deviceID = 0;
uint32_t DeviceSettings::propID = 0;

#endif //DEVICESETTINGS_H
