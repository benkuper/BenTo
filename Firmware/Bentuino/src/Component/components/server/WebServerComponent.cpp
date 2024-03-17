ImplementSingleton(WebServerComponent);

bool WebServerComponent::initInternal(JsonObject o)
{

    AddBoolParamConfig(sendFeedback);

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

#if USE_ASYNC_WEBSOCKET
    server.addHandler(&ws);
    ws.onEvent(std::bind(&WebServerComponent::onAsyncWSEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
#else
    ws.onEvent(std::bind<void>(&WebServerComponent::onWSEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
#endif

#ifdef USE_OSC
    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
              {

        if (request->hasArg("HOST_INFO"))
        {
            DynamicJsonDocument doc(1000);
            JsonObject o = doc.to<JsonObject>();
            JsonObject eo = o.createNestedObject("EXTENSIONS");
            eo["ACCESS"] = true;
            eo["CLIPMODE"] = false;
            eo["CRITICAL"] = false;
            eo["RANGE"] = true;
            eo["TAGS"] = false;
            eo["TYPE"] = true;
            eo["UNIT"] = false;
            eo["VALUE"] = true;
            eo["LISTEN"] = true;
            eo["PATH_ADDED"] = true;
            eo["PATH_REMOVED"] = true;
            eo["PATH_RENAMED"] = true;
            eo["PATH_CHANGED"] = false;

            o["NAME"] = RootComponent::instance->deviceName;
            o["OSC_PORT"] = OSC_LOCAL_PORT;
            o["OSC_TRANSPORT"] = "UDP";
#if !USE_ASYNC_WEBSOCKET
            o["WS_PORT"] = 81;
#endif

            String jStr;
            serializeJson(doc, jStr);
            request->send(200, "application/json", jStr);
        }
        else
        {
            std::shared_ptr<bool> showConfig = std::make_shared<bool>(request->hasArg("config")?request->arg("config")=="1":true);
            std::shared_ptr<OSCQueryChunk> chunk = std::make_shared<OSCQueryChunk>(OSCQueryChunk(RootComponent::instance));
            AsyncWebServerResponse *response = request->beginChunkedResponse("application/json", [chunk, showConfig](uint8_t *buffer, size_t maxLen, size_t index) {
 
                if(chunk->nextComponent == nullptr) return 0;

                // DBG("Fill chunk, config = " + String(*showConfig));

                chunk->nextComponent->fillChunkedOSCQueryData(chunk.get(), *showConfig);
                
                
                sprintf((char*)buffer, chunk->data.c_str());
                return (int)chunk->data.length();
                
            });

        request->send(response);
        } });
#endif

    // server.onNotFound(std::bind(&WebServerComponent::handleNotFound, this));

    // server.on("/", HTTP_ANY, std::bind(&WebServerComponent::handleQueryData, this));
    // server.on("/settings", HTTP_ANY, std::bind(&WebServerComponent::handleSettings, this));
    // server.on("/uploadFile", HTTP_POST, std::bind(&WebServerComponent::returnOK, this), std::bind(&WebServerComponent::handleFileUpload, this));

#if USE_FILES
    server.on(
        "/uploadFile", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200); },
        std::bind(&WebServerComponent::handleFileUpload,
                  this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

    if (FilesComponent::instance->useInternalMemory)
    {
        server.serveStatic("/edit", SPIFFS, "/server/edit.html");
        server.serveStatic("/upload", SPIFFS, "/server/upload.html");
        server.serveStatic("/server/", SPIFFS, "/server");
    }
    else
    {
        server.serveStatic("/edit", SD, "/server/edit.html");
        server.serveStatic("/upload", SD, "/server/upload.html");
        server.serveStatic("/server/", SD, "/server");
    }
#endif

    return true;
}

void WebServerComponent::updateInternal()
{
#if USE_ASYNC_WEBSOCKET
    // server.handleClient();
    ws.cleanupClients();
#else
    ws.loop();
#endif
}

void WebServerComponent::clearInternal()
{
#if USE_ASYNC_WEBSOCKET
    ws.closeAll();
#else
    ws.disconnect();
#endif
}

// SERVER
void WebServerComponent::onEnabledChanged()
{
    setupConnection();
}

void WebServerComponent::setupConnection()
{
    bool shouldConnect = enabled && WifiComponent::instance->state == WifiComponent::Connected;

    if (shouldConnect)
    {
        NDBG("Start HTTP Server");
        server.begin();
        NDBG("HTTP server started");

#if !USE_ASYNC_WEBSOCKET
        ws.begin();
#endif
    }
    else
    {
        // server.stop();
        // NDBG("HTTP server closed");
    }
}

void WebServerComponent::handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    DBG("Server File upload Client:" + request->client()->remoteIP().toString() + " " + request->url());

#if USE_FILES
    String dest = "";
    if (filename.endsWith(".wasm"))
        dest = "/scripts";
    else if (filename.endsWith(".colors") || filename.endsWith(".meta"))
        dest = "/playback";
    else if (filename.endsWith(".seq"))
        dest = "/playback";

    if (dest == "")
    {
        dest = request->hasArg("folder") ? request->arg("folder") : "";
    }

    dest += "/" + filename;

    if (!index)
    {
        // open the file on first call and store the file handle in the request object
        DBG("Upload Start: " + String(dest));
        request->_tempFile = FilesComponent::instance->openFile(dest, true, true);
    }

    if (len)
    {
        // stream the incoming chunk to the opened file
        DBG("Writing file: " + String(dest) + " index=" + String(index) + " len=" + String(len));
        request->_tempFile.write(data, len);

        // uploadedBytes += request->currentSize.currentSize;
        // float p = uploadedBytes * 1.0f / 1000000;
        // DBG("Upload progression... " + String((int)(p * 100)) + "%");
        // if (uploadedBytes % 8000 < 4000)
        // {
        //     var data[1];
        //     data[0] = p;
        //     sendEvent(Uploading, data, 1);
        // }
    }

    if (final)
    {
        // close the file handle as the upload is now done
        DBG("Upload Complete: " + String(dest) + ",size: " + String(index + len));
        request->_tempFile.close();
        // request->redirect("/");
    }
#endif
}

#if USE_ASYNC_WEBSOCKET
void WebServerComponent::onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                                   void *arg, uint8_t *data, size_t len)
{
    // DBG("WS Event");
    switch (type)
    {
    case WS_EVT_CONNECT:
        DBG("WebSocket client " + String(client->id()) + "connected from " + String(client->remoteIP().toString()));
        break;
    case WS_EVT_DISCONNECT:
        DBG("WebSocket client " + String(client->id()) + " disconnected");
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void WebServerComponent::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len)
    {
        data[len] = 0;
        // if (strcmp((char*)data, "toggle") == 0) {
        //   ledState = !ledState;
        //   notifyClients();
        // }

        if (info->opcode == WS_TEXT)
        {
            DBG("Message : " + String((char *)data));
        }
        else if (info->opcode == WS_BINARY)
        {
#ifdef USE_OSC
            OSCMessage msg;
            msg.fill(data, len);
            if (!msg.hasError())
            {
                // DBG("Got websocket OSC message");

                char addr[64];
                msg.getAddress(addr);
                tmpExcludeParam = String(addr);

                OSCComponent::instance->processMessage(msg);

                tmpExcludeParam = "";
            }
#endif
        }
    }
}
#else
void WebServerComponent::onWSEvent(uint8_t id, WStype_t type, uint8_t *data, size_t len)
{

    switch (type)
    {
    case WStype_DISCONNECTED:
        DBG("WebSocket client " + String(id) + "connected from " + StringHelpers::ipToString(ws.remoteIP(id)));
        break;
    case WStype_CONNECTED:
        DBG("WebSocket client " + String(id) + "connected from " + StringHelpers::ipToString(ws.remoteIP(id)));
        // webSocket.sendTXT(id, "Connected");
        break;

    case WStype_TEXT:
        DBG("Text received from " + String(id) + " > " + String((char *)data));

        // send message to client
        // webSocket.sendTXT(num, "message here");

        // send data to all connected clients
        // webSocket.broadcastTXT("message here");
        break;

    case WStype_BIN:
    {

        DBG("Got binary");
#ifdef USE_OSC
        OSCMessage msg;
        msg.fill(data, len);
        if (!msg.hasError())
        {
            // DBG("Got websocket OSC message");

            char addr[64];
            msg.getAddress(addr);
            tmpExcludeParam = String(addr);

            OSCComponent::instance->processMessage(msg);

            tmpExcludeParam = "";
        }
    }
#endif
    // send message to client
    // webSocket.sendBIN(num, payload, length);
    break;

    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    case WStype_PING:
    case WStype_PONG:
        break;

    default:
        break;
    }
}
#endif

// void WebServerComponent::sendParameterFeedback(Component *c, Parameter *param)
// {
// #ifdef USE_OSC
//     var v = param->getOSCQueryFeedbackData();
//     OSCMessage msg = OSCComponent::createMessage(c->getFullPath(), param->name, &v, 1, false);

//     char addr[64];
//     msg.getAddress(addr);
//     if (String(addr) == tmpExcludeParam)
//         return;

//     wsPrint.flush();
//     msg.send(wsPrint);
//     ws.binaryAll(wsPrint.data, wsPrint.index);
// #endif
// }

void WebServerComponent::sendParamFeedback(Component *c, String pName, var *data, int numData)
{
#ifdef USE_OSC

    OSCMessage msg = OSCComponent::createMessage(c->getFullPath(), pName, data, numData, false);

    char addr[64];
    msg.getAddress(addr);
    if (String(addr) == tmpExcludeParam)
        return;

    wsPrint.flush();
    msg.send(wsPrint);

#if USE_ASYNC_WEBSOCKET
    if (!ws.availableForWriteAll())
        return;
    ws.binaryAll(wsPrint.data, wsPrint.index);
#else
    ws.broadcastBIN(wsPrint.data, wsPrint.index);
#endif
#endif
}