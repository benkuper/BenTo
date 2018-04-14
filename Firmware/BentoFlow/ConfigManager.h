#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <ArduinoJson.h>
#include "FS.h"

class ConfigManager
{
public:
  ConfigManager();

  JsonObject& loadConfig(const char * path) {

    Serial.print("Load config file ");
    Serial.print(path);
    Serial.println("...");
    
    File configFile = SPIFFS.open(path, "r");
    if (!configFile) {
      Serial.println("Failed to open config file");
      return JsonObject::invalid();
    }
  
    size_t size = configFile.size();
    if (size > 1024) {
      Serial.println("Config file size is too large");
      return JsonObject::invalid();
    }
  
    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);
  
    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);
  
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());
  
    if (!json.success()) {
      Serial.println("Failed to parse config file");
      //return nullptr;
    }

    Serial.println("Config file loaded.");
    return json;
  }

  bool saveConfig(const char * path, JsonObject &json) {
    StaticJsonBuffer<200> jsonBuffer;
    
    File configFile = SPIFFS.open(path, "w");
    if (!configFile) {
      Serial.println("Failed to open config file for writing");
      return false;
    }
  
    json.printTo(configFile);
    return true;

    Serial.print("Saved config to file ");
    Serial.print(path);
    Serial.println("");
  }
};

#endif
