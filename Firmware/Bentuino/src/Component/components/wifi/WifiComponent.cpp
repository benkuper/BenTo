ImplementSingleton(WifiComponent)

void WifiComponent::initInternal()
{
    NDBG("Init");

    state = Off;

    ssid = addParameter<String>("ssid", LoadString("ssid", "freeboxrondrouge"));
    pass = addParameter<String>("pass", LoadString("pass","moufledelenfer"));

    connect();
}

void WifiComponent::updateInternal()
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
                setState(Connected);
                timeAtConnect = -1;
        }

            if (curTime > timeAtConnect + connectionTimeout)
            {
                setState(ConnectionError);
            }
            break;

        case Connected: 
            if (!WiFi.isConnected())
            {
                NDBG("Lost connection ! will reconnect soon...");
                if(timeAtConnect == -1)
                {
                    timeAtConnect = millis();
                }else if(curTime > timeAtConnect + connectionTimeout)
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

    NDBG("Connecting to " + ssid->val + " : " + pass->val + "...");
    WiFi.begin(ssid->val.c_str(), pass->val.c_str());

    setState(Connecting);
}

void WifiComponent::disconnect()
{
    WiFi.disconnect();
    setState(Off);
}

String WifiComponent::getIP() const
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