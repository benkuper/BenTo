ImplementSingleton(WifiComponent)

    bool WifiComponent::initInternal(JsonObject o)
{
    state = Off;

    // AddAndSetParameter(ssid);
    // AddAndSetParameter(pass);
    // AddAndSetParameter(apOnNoWifi);

    AddStringParamConfig(ssid);
    AddStringParamConfig(pass);
#ifdef USE_ETHERNET
    AddIntParamConfig(mode);

    WiFi.onEvent(std::bind(&WifiComponent::WiFiEvent, this, std::placeholders::_1));
#endif

    connect();

    return true;
}

void WifiComponent::updateInternal()
{
#ifdef USE_ETHERNET
    if (mode == MODE_ETH)
        return;
#endif

    long curTime = millis();
    if (curTime > lastConnectTime + timeBetweenTries)
    {
        switch (state)
        {
        case Connecting:

#if defined ESP32
            if (WiFi.isConnected())
#elif defined ESP8266
            if (WiFi.status() == WL_CONNECTED)
#endif
            {
                setState(Connected);

                timeAtConnect = -1;
            }

            if (curTime > timeAtConnect + connectionTimeout)
            {
                NDBG("Connection Error");
                setState(ConnectionError);
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

void WifiComponent::clearInternal()
{
    disconnect();
}

void WifiComponent::setState(ConnectionState s)
{
    if (state == s)
        return;

    state = s;
    switch (state)
    {
    case Connected:
#ifdef USE_ETHERNET
        NDBG("Connected to ethernet with IP " + getIP());

#else
        NDBG("Connected to wifi " + ssid + " : " + pass + " with IP " + getIP());
#endif
        break;

    case ConnectionError:
        NDBG("Connection error");
        break;
    }

    timeAtStateChange = millis();
    sendEvent(ConnectionStateChanged);
}

void WifiComponent::connect()
{
    lastConnectTime = millis();
    timeAtConnect = millis();

    if (state == Connected || state == Hotspot)
        WiFi.disconnect();

    wifi_mode_t wMode = WIFI_STA;
    switch (mode)
    {
    case MODE_STA:
        wMode = WIFI_STA;
        break;
    case MODE_AP:
        wMode = WIFI_AP;
        break;
    case MODE_AP_STA:
        wMode = WIFI_AP_STA;
        break;

#ifdef USE_ETHERNET
    case MODE_ETH:
        wMode = WIFI_OFF;
        break;
    case MODE_ETH_STA:
        wMode = WIFI_STA;
        break;
#endif
    }

    WiFi.mode(wMode);

#ifdef ESP32
#endif

    setState(Connecting);

#ifdef USE_ETHERNET
    if (mode == MODE_ETH || mode == MODE_ETH_STA)
    {
        NDBG("Connecting to ethernet...");
        ETH.begin();
    }
#endif

    if (wMode != WIFI_OFF)
    {
        NDBG("Connecting to wifi " + ssid + " : " + pass + "...");
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        WiFi.setSleep(false);
        WiFi.setTxPower(WIFI_POWER_19dBm);
        WiFi.begin(ssid.c_str(), pass.c_str());
    }
}

void WifiComponent::disable()
{
    WiFi.disconnect();
    setState(Disabled);
}

void WifiComponent::disconnect()
{
    WiFi.disconnect();
    setState(Off);
}

#ifdef USE_ETHERNET
void WifiComponent::WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {

    case ARDUINO_EVENT_ETH_START:
        ETH.setHostname(DeviceID.c_str());
        break;

    case ARDUINO_EVENT_ETH_CONNECTED:
        NDBG("Ethernet connected, getting IP...");
        // setState(Connected);
        break;

    case ARDUINO_EVENT_ETH_GOT_IP:
        setState(Connected);
        break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
        setState(ConnectionError);
        break;

    case ARDUINO_EVENT_ETH_STOP:
        setState(ConnectionError);
        break;

    default:
        break;
    }
}
#endif

String WifiComponent::getIP() const
{
    if (state == Connected)
#ifdef USE_ETHERNET
        return StringHelpers::ipToString(ETH.localIP());
#else
        return StringHelpers::ipToString(WiFi.localIP());
#endif

    else if (state == Hotspot)
        return StringHelpers::ipToString(WiFi.softAPIP());

    return "[noip]";
}