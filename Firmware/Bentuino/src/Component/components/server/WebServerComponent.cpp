ImplementSingleton(WebServerComponent)

    bool WebServerComponent::initInternal(JsonObject o)
{
    // server.cors(); // This is the magic

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        DynamicJsonDocument doc(32000);
        JsonObject o = doc.to<JsonObject>();
        RootComponent::instance->fillOSCQueryData(o);

        String jStr;
        serializeJson(doc, jStr);
        request->send(200, "application/json", jStr); });

    // server.onNotFound(std::bind(&WebServerComponent::handleNotFound, this));

    // server.on("/", HTTP_ANY, std::bind(&WebServerComponent::handleQueryData, this));
    // server.on("/settings", HTTP_ANY, std::bind(&WebServerComponent::handleSettings, this));
    // server.on("/uploadFile", HTTP_POST, std::bind(&WebServerComponent::returnOK, this), std::bind(&WebServerComponent::handleFileUpload, this));

    server.on(
        "/uploadFile", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200); },
        std::bind(&WebServerComponent::handleFileUpload,
                  this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

    server.serveStatic("/edit", SD, "/server/edit.html");
    server.serveStatic("/upload", SD, "/server/upload.html");
    server.serveStatic("/server/", SD, "/server");

    ws.onEvent( std::bind(&WebServerComponent::onWSEvent,
                  this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

    server.addHandler(&ws);

    return true;
}

void WebServerComponent::updateInternal()
{
    // server.handleClient();
      ws.cleanupClients();
}

void WebServerComponent::clearInternal()
{
}

// SERVER
void WebServerComponent::onEnabledChanged()
{
    setupConnection();
}

void WebServerComponent::setupConnection()
{
    bool shouldConnect = enabled.boolValue() && WifiComponent::instance->state == WifiComponent::Connected;

    if (shouldConnect)
    {
        server.begin();
        NDBG("HTTP server started");
    }
    else
    {
        // server.stop();
        NDBG("HTTP server closed");
    }
}

void WebServerComponent::handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    DBG("Server File upload Client:" + request->client()->remoteIP().toString() + " " + request->url());

    String dest = "";
    if (filename.endsWith(".wasm"))
        dest = "/scripts";
    else if (filename.endsWith(".colors") || filename.endsWith(".meta"))
        dest = "/bake";
    else if (filename.endsWith(".seq"))
        dest = "/sequences";

    if (dest == "")
    {
        dest = request->hasArg("folder") ? request->arg("folder") : "";
    }

    dest += "/" + filename;

    if (!index)
    {
        // open the file on first call and store the file handle in the request object
        DBG("Upload Start: " + String(filename));
        request->_tempFile = FilesComponent::instance->openFile(dest, true, true);
    }

    if (len)
    {
        // stream the incoming chunk to the opened file
        DBG("Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len));
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
        DBG("Upload Complete: " + String(filename) + ",size: " + String(index + len));
        request->_tempFile.close();
        request->redirect("/");
    }
}

void WebServerComponent::onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    DBG("WS Event");
    switch (type)
    {
    case WS_EVT_CONNECT:
        DBG("WebSocket client " + String(client->id()) + "connected from " + String(client->remoteIP().toString()));
        break;
    case WS_EVT_DISCONNECT:
        DBG("WebSocket client "+String(client->id())+" disconnected");
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void WebServerComponent::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    // if (strcmp((char*)data, "toggle") == 0) {
    //   ledState = !ledState;
    //   notifyClients();
    // }

    DBG("Message : " + String((char*)data));
  }
}

// String dest = "";

// HTTPUpload &upload = server.upload();

// if (upload.filename.endsWith(".wasm"))
//     dest = "/scripts";
// else if (upload.filename.endsWith(".colors") || upload.filename.endsWith(".meta"))
//     dest = "/bake";
// else if (upload.filename.endsWith(".seq"))
//     dest = "/sequences";

// if (dest == "")
// {
//     dest = server.hasArg("folder") ? server.arg("folder") : "";
//     // NDBG("File " + String(upload.filename) + " not handled");
//     // return;
// }

// dest += "/" + upload.filename;

// if (upload.status == UPLOAD_FILE_START)
// {
//     NDBG("Upload destination : " + dest);

//     uploadedBytes = 0;
//     // totalBytes = server.header("Content-Length").toInt();

//     uploadingFile = FilesComponent::instance->openFile(dest, true, true);
//     if (uploadingFile)
//     {
//         var data[1];
//         data[0] = (char *)uploadingFile.name();
//         sendEvent(UploadStart, data, 1);
//     }
//     else
//     {
//         NDBG("ERROR WHEN CREATING THE FILE");
//     }

//     isUploading = true;
// }
// else if (upload.status == UPLOAD_FILE_WRITE)
// {
//     if (uploadingFile)
//     {
//         if (uploadedBytes == 0 && upload.buf[0] == 13 && upload.buf[1] == 10)
//         {
//             NDBG("Remove new line nonsense");
//             uploadingFile.write(upload.buf + 2, upload.currentSize - 2);
//         }
//         else
//         {
//             uploadingFile.write(upload.buf, upload.currentSize);
//         }

//         uploadedBytes += upload.currentSize;
//         float p = uploadedBytes * 1.0f / 1000000;
//         NDBG("Upload progression... " + String((int)(p * 100)) + "%");
//         if (uploadedBytes % 8000 < 4000)
//         {
//             var data[1];
//             data[0] = p;
//             sendEvent(Uploading, data, 1);
//         }
//     }
// }
// else if (upload.status == UPLOAD_FILE_END)
// {
//     DBG("Upload file end");
//     if (uploadingFile)
//     {
//         String n = uploadingFile.name();
//         NDBG("Upload total size " + String(upload.totalSize) + " < > " + String(uploadingFile.size()));
//         uploadingFile.close();
//         NDBG("File closed");

//         DBG("Send event");
//         //var data[1];
//         //data[0] = var(uploadingFile.name());
//         sendEvent(UploadDone);
//         isUploading = false;
//     }
//     else
//     {
//         NDBG("Upload finish ERROR");
//         isUploading = false;
//     }
// }
// else if (upload.status == UPLOAD_FILE_ABORTED)
// {
//     NDBG("ABOORT !!!!!!!!!!");
//     uploadingFile.close();
//     sendEvent(UploadCanceled);
//     isUploading = false;
// }
// }

// void WebServerComponent::returnOK()
// {
//     server.send(200, "text/plain", "ok");
// }

// void WebServerComponent::returnFail(String msg)
// {
//     NDBG("Failed here");
//     server.send(500, "text/plain", msg + "\r\n");
// }