#include "Common.h"

#ifdef ESP32
#include <WiFi.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#endif

String getDeviceID() 
{
    byte mac[6];
    WiFi.macAddress(mac);

    String result = "";
    for(int i=0;i<6;i++)
    {
        result += (i > 0?":":"") + String(mac[i], HEX);
    }

    result.toUpperCase();
    return result;
    
    //uint64_t did = ESP.getEfuseMac();
    //return String((uint32_t)(did >> 32)) + String((uint32_t)(did & 0xffffffff));
}