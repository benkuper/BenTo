/* 

  SettingsManager

  Copyright (C) 2017 by Sergiu Toporjinschi <sergiu dot toporjinschi at gmail dot com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation version 3.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://spdx.org/licenses/GPL-3.0-only.html>.

  All rights reserved

*/

//#include "../DebugHelpers.h"

#ifndef ESP32

#include "SettingsManager.h"


/**
    Reads the content of settings file given by path/name
*/
int SettingsManager::readSettings(const char *fileName) {
  Serial.println("Reading settings from: " + String(fileName));
  unsigned int loaded = SM_SUCCESS;
  openSPIFFS();
  File file = SPIFFS.open(fileName, "r");
  if (!file) {
     Serial.println("Could not open file");
    SPIFFS.end();
    loaded = SM_ERROR;
  } else {
    char js[JSON_LEN] = {0};
    getFileContent(js, file);
     Serial.println("File content : "+String(js));
    loaded = loadJson(js);
    file.close();
    SPIFFS.end();
  }
   Serial.println("Closing file");
  return loaded;
}

/**
    Writes the content of settings to a file given by path/name
*/
int SettingsManager::writeSettings(const char *fileName) {
  return writeSettings(fileName, doc.as<JsonVariant>());
}

/**
    Writes the content of settings to a file given by path/name
*/
int SettingsManager::writeSettings(const char *fileName, JsonVariant conf) {
  // DBG("Writing settings to: "+String(fileName));
  openSPIFFS();

  File file = SPIFFS.open(fileName, "w");
  if (!file) {
    // DBG("Could not write in file");
    SPIFFS.end();
    return SM_ERROR;
  } else {
    serializeJson(conf, file);
    delay(20);
    // DBG("File written");
  }
  file.close();
  SPIFFS.end();
  // DBG("File and SPIFFS closed");
  return SM_SUCCESS;
}

/**
   Reads the file content
*/
void SettingsManager::getFileContent(char *content, File &file) {
  char jsonChars[7] = "{},:[]";
  char lastChr = '\0';
  while (file.available()) {
    char chr = (char)file.read();
    if (chr == 10 || chr == '\r' || chr == '\t' || (strchr(jsonChars, lastChr) != NULL && chr == ' ') || (int)chr == 255) {
      continue;
    }
    if ((int)lastChr == 32 && strchr(jsonChars, chr) != NULL) {
      content[strlen(content) - 1] = '\000';
    }
    content[strlen(content)] = chr;
    content[strlen(content) + 1] = '\000';
    lastChr = chr;
  }
}

/**
   Loads a json and is stored in json structure
*/
int SettingsManager::loadJson(const char *payload) {
  doc.clear();
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    // DBG("Could not deserialize payload: "+String(err.c_str()));
    return SM_ERROR;
  }
  root = doc.as<JsonVariant>();
  return SM_SUCCESS;
}

/**
   Open file
*/
void SettingsManager::openSPIFFS() {
  if (!SPIFFS.begin()) {
    delay(20);
    // DBG("Could not mount SPIFFS file system");
  } else {
    // DBG("SPIFFS file system, open");
  }
}

/**
   Returns the JsonVariant of a given key
*/
JsonVariant SettingsManager::getJsonVariant(const char *key, bool addIfMissing) {
  // DBG("-> Searching for key: "+String(key));
  //Maybe i'm lucky ...
  JsonVariant val = root[key];
  if (!val.isNull()) {
    // DBG("Key found in root: "+String(key));
    return val;
  }
  char _key[100] = {0};
  strcpy(_key, key);
  char *k = strchr(_key, '.');

  //if it does not have any . then is not existing
  if (k == nullptr) {
    if (addIfMissing) {
      // DBG("Adding not existing key: "+String(key));
      JsonVariant result = root[key].to<JsonVariant>();
      return result;
    } else {
      // DBG("Key not found "+String(key));
      return JsonVariant();
    }
  }

  //for sure it has at least one .
  char *crs = &_key[0];
  JsonVariant node = root;
  while (k != nullptr) {
    //replace the . with \0 to split the string
    k[0] = '\0';
    if (strlen(crs) > 0) {
      if (!node[crs].isNull()) {
        node = node[crs];
      } else if (!addIfMissing) {
        // DBG("Key not found: "+String(crs));
        return JsonVariant();
      } else {
        // DBG("Adding not existing key: "+String(crs));
        node = node[crs].to<JsonVariant>();
      }
    }
    k++;
    crs = k;
    k = strchr(crs, '.');
    if (k == nullptr) {
      if (node[crs].isNull() && addIfMissing) {
        // DBG("Adding not existing key: "+String(crs));
        node = node[crs].to<JsonVariant>();
      } else {
        node = node[crs];
      }
      break;
    }
  }
  return node;
};

/**
   Returns the JsonObject stored to a specific key
*/
JsonObject SettingsManager::getJsonObject(const char *key, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (!item.isNull()) {
    return item.as<JsonObject>();
  } else if (addIfMissing) {
    return item[key].to<JsonVariant>();
  } else {
    return JsonObject();
  }
}

/**
   Returns the JsonArray stored to a specific key
*/
JsonArray SettingsManager::getJsonArray(const char *key, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<JsonArray>();
  } else if (addIfMissing) {
    return item[key].to<JsonVariant>();
  } else {
    return JsonArray();
  }
}

signed int SettingsManager::getInt(const char *key, const signed int defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<signed int>();
  } else {
    return defaultValue;
  }
}

unsigned int SettingsManager::getUInt(const char *key, const unsigned int defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<unsigned int>();
  } else {
    return defaultValue;
  }
}

signed short SettingsManager::getShort(const char *key, const signed short defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<signed short>();
  } else {
    return defaultValue;
  }
}

unsigned short SettingsManager::getUShort(const char *key, const unsigned short defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<unsigned short>();
  } else {
    return defaultValue;
  }
}

signed long SettingsManager::getLong(const char *key, const signed long defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<signed long>();
  } else {
    return defaultValue;
  }
}

unsigned long SettingsManager::getULong(const char *key, const unsigned long defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<unsigned long>();
  } else {
    return defaultValue;
  }
}

// char SettingsManager::getCChar(const char *key, const char defaultValue) {
//   JsonVariant item = getJsonVariant(key);
//   if (!item.isNull()) {
//     const char *x = item.as<const char *>();
//     return x[0];
//   } else {
//     return defaultValue;
//   }
// }

// signed char SettingsManager::getSChar(const char *key, const signed char defaultValue) {
//   JsonVariant item = getJsonVariant(key);
//   if (!item.isNull()) {
//     return item.as<signed char>();
//   } else {
//     return defaultValue;
//   }
// }

// unsigned char SettingsManager::getUChar(const char *key, const unsigned char defaultValue) {
//   JsonVariant item = getJsonVariant(key);
//   if (!item.isNull()) {
//     return item.as<unsigned char>();
//   } else {
//     return defaultValue;
//   }
// }

// const char *SettingsManager::getChar(const char *key, const char *defaultValue) {
//   JsonVariant item = getJsonVariant(key);
//   if (!item.isNull()) {
//     return item.as<char *>();
//   } else {
//     return defaultValue;
//   }
// }

String SettingsManager::getString(const char *key, String defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<String>();
  } else {
    return defaultValue;
  }
}

float SettingsManager::getFloat(const char *key, const float defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<float>();
  } else {
    return defaultValue;
  }
}

double SettingsManager::getDouble(const char *key, const double defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<double>();
  } else {
    return defaultValue;
  }
}

bool SettingsManager::getBool(const char *key, const bool defaultValue) {
  JsonVariant item = getJsonVariant(key);
  if (!item.isNull()) {
    return item.as<bool>();
  } else {
    return defaultValue;
  }
}

int SettingsManager::setInt(const char *key, const signed int value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}

int SettingsManager::setShort(const char *key, const signed short value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}

int SettingsManager::setLong(const char *key, const signed long value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}
/*
int SettingsManager::setCChar(const char *key, const char value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}

int SettingsManager::setChar(const char *key, const char *value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}
*/

int SettingsManager::setString(const char *key, String value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}

int SettingsManager::setFloat(const char *key, const float value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}

int SettingsManager::setDouble(const char *key, const double value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}

int SettingsManager::setBool(const char *key, const bool value, bool addIfMissing) {
  JsonVariant item = getJsonVariant(key, addIfMissing);
  if (addIfMissing || !item.isNull()) {
    return item.set(value) ? SM_SUCCESS : SM_ERROR;
  } else {
    // DBG("null node");
    return SM_KEY_NOT_FOUND;
  }
}

#endif