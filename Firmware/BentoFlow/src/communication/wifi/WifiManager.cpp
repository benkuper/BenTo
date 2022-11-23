
#include "WifiManager.h"

WifiManager::WifiManager() : Component("wifi"),
                             state(Off),
                             lastConnectTime(0)
{
}

void WifiManager::init()
{
    NDBG("Init");

#if defined USE_SETTINGS_MANAGER
    // init once with a json if it doesn't exist yet
    prefs.readSettings(String("/" + name + ".json").c_str());
    String ssid = prefs.getString("ssid");
    String pass = prefs.getString("pass");
    prefs.loadJson(String("{\"ssid\":\"" + ssid + "\",\"pass\":\"" + pass + "\"}").c_str());
    prefs.writeSettings(String("/" + name + ".json").c_str());
#endif

    connect();
}

void WifiManager::update()
{
    long curTime = millis();
    if (curTime > lastConnectTime + timeBetweenTries)
    {
        switch (state)
        {
        case Connecting:
        {

#if defined ESP32
            if (WiFi.isConnected())
#elif defined ESP8266
            if (WiFi.status() == WL_CONNECTED)
#endif
            {
                setState(Connected);
                timeAtConnect = -1;
            }
            else
            {
                if (curTime > timeAtConnect + connectionTimeout)
                {
                    setState(ConnectionError);
                }
            }
        }
        break;

        case Connected:
            if (!WiFi.isConnected())
            {
                NDBG("Lost connection ! will reconnect soon...");
                if (timeAtConnect == -1)
                {
                    timeAtConnect = millis();
                }
                else if (curTime > timeAtConnect + connectionTimeout)
                {
                    connect();
                }
            }
            break;

        default:
            break;
        }
        }
    }

    void WifiManager::setState(ConnectionState s)
    {
        if (state == s)
            return;

        state = s;
        sendEvent();
    }

    void WifiManager::connect()
    {
        lastConnectTime = millis();
        timeAtConnect = millis();

        if (state == Connected || state == Hotspot)
            WiFi.disconnect();

        WiFi.mode(WIFI_STA);

#ifdef ESP32
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        WiFi.setSleep(false);
        WiFi.setTxPower(WIFI_POWER_19dBm);
#endif

#ifdef USE_PREFERENCES
        prefs.begin(name.c_str(), true);
        String ssid = prefs.getString("ssid", "");
        String pass = prefs.getString("pass", "");
        prefs.end();
#elif defined USE_SETTINGS_MANAGER
        prefs.readSettings(String("/" + name + ".json").c_str());
        String ssid = prefs.getString("ssid");
        String pass = prefs.getString("pass");
#endif

        NDBG("Connecting to " + ssid + " : " + pass + "...");
        WiFi.begin(ssid.c_str(), pass.c_str());

        setState(Connecting);
    }

    void WifiManager::disconnect()
    {
        WiFi.disconnect();
        setState(Off);
    }

    void WifiManager::disable()
    {
        WiFi.disconnect();
        setState(Disabled);
    }

    void WifiManager::saveWifiConfig(String ssid, String pass)
    {
#ifdef USE_PREFERENCES
        prefs.begin(name.c_str());
        prefs.putString("ssid", ssid);
        prefs.putString("pass", pass);
        prefs.end();
#elif defined USE_SETTINGS_MANAGER
        prefs.setString("ssid", ssid);
        prefs.setString("pass", pass);
        prefs.writeSettings(String("/" + name + ".json").c_str());
#endif

        connect();
    }

    String WifiManager::getIP()
    {
        if (state == Connected)
            return String(WiFi.localIP()[0]) +
                   "." + String(WiFi.localIP()[1]) +
                   "." + String(WiFi.localIP()[2]) +
                   "." + String(WiFi.localIP()[3]);

        else if (state == Hotspot)
            return String(WiFi.softAPIP()[0]) +
                   "." + String(WiFi.softAPIP()[1]) +
                   "." + String(WiFi.softAPIP()[2]) +
                   "." + String(WiFi.softAPIP()[3]);

        return "[noip]";
    }

    bool WifiManager::handleCommand(String command, var * data, int numData)
    {
        if (checkCommand(command, "setCredentials", numData, 2))
        {
            saveWifiConfig(data[0].stringValue(), data[1].stringValue());
            return true;
        }

        return false;
    }