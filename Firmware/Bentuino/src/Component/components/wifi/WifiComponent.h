#pragma once

#ifdef ESP32
#include <WiFi.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#endif

DeclareComponentSingleton(Wifi, "wifi", )

    enum ConnectionState { Off,
                           Connecting,
                           Connected,
                           ConnectionError,
                           Disabled,
                           Hotspot,
                           PingAlive,
                           PingDead,
                           CONNECTION_STATES_MAX };

const String connectionStateNames[CONNECTION_STATES_MAX]{"Off", "Connecting", "Connected", "ConnectionError", "Disabled", "Hotspot", "PingAlive", "PingDead"};

const long timeBetweenTries = 500;    // ms
const long connectionTimeout = 10000; // ms
long timeAtConnect;
long lastConnectTime;
long timeAtStateChange;

ConnectionState state;

DeclareStringParam(ssid, "");
DeclareStringParam(pass, "");

// Parameter ssid{"ssid", "", var(), var(), true};
// Parameter pass{"pass", "", var(), var(), true};
// Parameter apOnNoWifi{"apOnNoWifi", true, var(), var(), true};

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void connect();
void disconnect();
void setAP();

void disable();
void setState(ConnectionState s);

String getIP() const;

HandleSetParamInternalStart
    CheckAndSetParam(ssid);
CheckAndSetParam(pass);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(ssid);
FillSettingsParam(pass);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryStringParam(ssid);
FillOSCQueryStringParam(pass);
FillOSCQueryInternalEnd;

DeclareComponentEventTypes(ConnectionStateChanged);
DeclareComponentEventNames("ConnectionStateChanged");

EndDeclareComponent