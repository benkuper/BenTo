const char wifiSettingsForm[] =
  "<!DOCTYPE HTML>"
  "<html>"
  "<head>"
  "<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
  "<title>BenTo Wifi Settings</title>"
  "<style>"
  "\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
  "</style>"
  "</head>"
  "<body>"
  "<h1>BenTo WiFi Settings</h1>"
  "<FORM action=\"/\" method=\"post\">"
  "<P>"
  "LED<br>"
  "<input type=\"text\" name=\"ssid\">SSID<BR>"
  "<input type=\"text\" name=\"pass\">Password<BR>"
  "<input type=\"submit\" value=\"Send\">"
  "</P>"
  "</FORM>"
  "</body>"
  "</html>";


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

#define SERIAL_DEBUG 1
#define USE_BONJOUR 0

class WifiSettingsServer
{
public:

    String ssid = "";
    String pass = "";
    
    void init()
    {
      server.on("/", std::bind(&WifiSettingsServer::handleRoot,this));
      server.onNotFound(std::bind(&WifiSettingsServer::handleNotFound,this));
      
      #if SERIAL_DEBUG
            Serial.println("TCP server started");
      #endif
      
      #if USE_BONJOUR
            // Add service to MDNS-SD
            //mdns.addService("http", "tcp", 80);
      #endif

      server.begin();
    }

    void update()
    {
      server.handleClient();
    }

    void handleRoot()
    {
      if (server.hasArg("ssid") && server.hasArg("pass")) {
        handleSubmit();
      }
      else {
        server.send(200, "text/html", wifiSettingsForm);
      }
    }

    void returnFail(String msg)
    {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(500, "text/plain", msg + "\r\n");
    }

    void handleSubmit()
    {
      String ssidValue;
      String passValue;
      
      if (!server.hasArg("ssid") || !server.hasArg("pass")) return returnFail("BAD ARGS");
      ssidValue = server.arg("ssid");
      passValue = server.arg("pass");
      
      if (ssidValue != "" && passValue != "") {
        server.send(200, "text/html", wifiSettingsForm);
        saveSettings(ssidValue,passValue);
      }else {
        returnFail("Bad Form values");
      }
    }

    void returnOK()
    {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", "OK\r\n");
    }

    /*
       Imperative to turn the LED on using a non-browser http client.
       For example, using wget.
       $ wget http://esp8266webform/ledon
    */
    void saveSettings(String ssidVal, String passVal)
    {
      ssid = ssidVal;
      pass = passVal;
      Serial.println(String("Saving")+ssid+String(" : ")+pass);
    }

    void handleNotFound()
    {
      String message = "File Not Found\n\n";
      message += "URI: ";
      message += server.uri();
      message += "\nMethod: ";
      message += (server.method() == HTTP_GET) ? "GET" : "POST";
      message += "\nArguments: ";
      message += server.args();
      message += "\n";
      for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      }
      server.send(404, "text/plain", message);
    }
};

