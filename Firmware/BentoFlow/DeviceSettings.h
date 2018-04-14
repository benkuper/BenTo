#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H


class DeviceSettings
{
public :

  static uint32_t deviceID;
  
  DeviceSettings()
  {
  
  }
  
  void init()
  {
    deviceID = ESP.getChipId();
    
    #if SERIAL_DEBUG
    Serial.println("DeviceSettings init.");
    Serial.print("Device ID : ");
    Serial.println(deviceID);
    #endif
  }
};

uint32_t DeviceSettings::deviceID = 0;

#endif //DEVICESETTINGS_H
