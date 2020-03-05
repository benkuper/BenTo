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

#pragma once
#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#  include "SPIFFS.h"
#endif
#ifdef ARDUINO_ARCH_ESP8266
#  include "FS.h"
#endif

#include "Macro.h"
#include "../ArduinoJson/ArduinoJson.h"
#include <map>

class SettingsManager {
 public:
  SettingsManager(){};
  int readSettings(const char *fileName);
  int readSettings(const String fileName) {
    return readSettings(fileName.c_str());
  };
  int writeSettings(const char *fileName);
  int writeSettings(const char *fileName, JsonVariant conf);

  signed int getInt(const char *key, const signed int defaultValue = 0);
  unsigned int getUInt(const char *key, const unsigned int defaultValue = 0);

  signed short getShort(const char *key, const signed short defaultValue = 0);
  unsigned short getUShort(const char *key, const unsigned short defaultValue = 0);

  signed long getLong(const char *key, const signed long defaultValue = 0L);
  unsigned long getULong(const char *key, const unsigned long defaultValue = 0L);

  char getCChar(const char *key, const char defaultValue = '\0');
  signed char getSChar(const char *key, const signed char defaultValue = '\0');
  unsigned char getUChar(const char *key, const unsigned char defaultValue = '\0');

  const char *getChar(const char *key, const char *defaultValue = '\0');
  String getString(const char *key, const String defaultValue = "");
  float getFloat(const char *key, const float defaultValue = 0.0f);
  double getDouble(const char *key, const double defaultValue = (double)(0.0));
  bool getBool(const char *key, const bool defaultValue = false);

  int setInt(const char *key, const signed int value, bool addIfMissing = 1);
  int setShort(const char *key, const signed short value, bool addIfMissing = 1);
  int setLong(const char *key, const signed long value, bool addIfMissing = 1);
  int setCChar(const char *key, const char value, bool addIfMissing = 1);
  int setChar(const char *key, const char *value, bool addIfMissing = 1);
  int setString(const char *key, const String value, bool addIfMissing = 1);
  int setFloat(const char *key, const float value, bool addIfMissing = 1);
  int setDouble(const char *key, const double value, bool addIfMissing = 1);
  int setBool(const char *key, const bool value, bool addIfMissing = 1);

  JsonVariant getJsonVariant(const char *key, bool addIfMissing = 0);
  JsonObject getJsonObject(const char *key, bool addIfMissing = 0);
  JsonArray getJsonArray(const char *key, bool addIfMissing = 0);

  JsonObject getRoot() {
    return root;
  };
  int loadJson(const char *payload);

 private:
  DynamicJsonDocument doc = DynamicJsonDocument(1300);
  JsonObject root;
  void getFileContent(char *content, File &file);
  void openSPIFFS();
};