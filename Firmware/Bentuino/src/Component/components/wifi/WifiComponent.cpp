ImplementSingleton(WifiComponent)

    bool WifiComponent::initInternal(JsonObject o)
{
    state = Off;

    // AddAndSetParameter(ssid);
    // AddAndSetParameter(pass);
    // AddAndSetParameter(apOnNoWifi);

    AddStringParamConfig(ssid);
    AddStringParamConfig(pass);

    connect();

    return true;
}

void WifiComponent::updateInternal()
{
    
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
                NDBG("Connected, local IP is " + getIP());
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
    timeAtStateChange = millis();
    sendEvent(ConnectionStateChanged);
}

void WifiComponent::connect()
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

    NDBG("Connecting to " + ssid + " : " + pass + "...");
    WiFi.begin(ssid.c_str(), pass.c_str());

    setState(Connecting);
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

String WifiComponent::getIP() const
{
    if (state == Connected)
        return StringHelpers::ipToString(WiFi.localIP());

    else if (state == Hotspot)
        return StringHelpers::ipToString(WiFi.softAPIP());

    return "[noip]";
}