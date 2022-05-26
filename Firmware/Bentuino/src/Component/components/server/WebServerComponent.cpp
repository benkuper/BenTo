ImplementSingleton(WebServerComponent)

    bool WebServerComponent::initInternal(JsonObject o)
{
    server.enableCORS(); // This is the magic
    server.onNotFound(std::bind(&WebServerComponent::handleNotFound, this));
    server.on("/", HTTP_ANY, std::bind(&WebServerComponent::handleQueryData, this));
    server.on("/settings", HTTP_ANY, std::bind(&WebServerComponent::handleSettings, this));
    server.on("/uploadFile", HTTP_POST, std::bind(&WebServerComponent::returnOK, this), std::bind(&WebServerComponent::handleFileUpload, this));
    server.serveStatic("/edit", SPIFFS, "/server/edit.html");
    server.serveStatic("/upload", SPIFFS, "/server/upload.html");
    return true;
}

void WebServerComponent::updateInternal()
{
    if (!enabled->boolValue())
        return;
    server.handleClient();
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
    bool shouldConnect = enabled->boolValue() && WifiComponent::instance->state == WifiComponent::Connected;

    if (shouldConnect)
    {
        server.begin();
        NDBG("HTTP server started");
    }
    else
    {
        server.close();
        NDBG("HTTP server closed");
    }
}

void WebServerComponent::handleFileUpload()
{
    String dest = "";

    HTTPUpload &upload = server.upload();

    if (upload.filename.endsWith(".wasm"))
        dest = "/scripts";
    else if (upload.filename.endsWith(".colors") || upload.filename.endsWith(".meta"))
        dest = "/bake";
    else if (upload.filename.endsWith(".seq"))
        dest = "/sequences";

    if (dest == "")
    {
        NDBG("File " + String(upload.filename) + " not handled");
        return;
    }
    dest += "/" + upload.filename;

    if (upload.status == UPLOAD_FILE_START)
    {
        NDBG("Upload destination : " + dest);

        uploadedBytes = 0;
        // totalBytes = server.header("Content-Length").toInt();

        uploadingFile = FilesComponent::instance->openFile(dest, true, true);
        if (uploadingFile)
        {
            var data[1];
            data[0] = (char *)uploadingFile.name();
            sendEvent(UploadStart, data, 1);
        }
        else
        {
            NDBG("ERROR WHEN CREATING THE FILE");
        }

        isUploading = true;
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (uploadingFile)
        {
            if (uploadedBytes == 0 && upload.buf[0] == 13 && upload.buf[1] == 10)
            {
                NDBG("Remove new line nonsense");
                uploadingFile.write(upload.buf + 2, upload.currentSize - 2);
            }
            else
            {
                uploadingFile.write(upload.buf, upload.currentSize);
            }

            uploadedBytes += upload.currentSize;
            float p = uploadedBytes * 1.0f / 1000000;
            NDBG("Upload progression... " + String((int)(p * 100)) + "%");
            if (uploadedBytes % 8000 < 4000)
            {
                var data[1];
                data[0] = p;
                sendEvent(Uploading, data, 1);
            }
        }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        DBG("Upload file end");
        if (uploadingFile)
        {
            String n = uploadingFile.name();
            NDBG("Upload total size " + String(upload.totalSize) + " < > " + String(uploadingFile.size()));
            uploadingFile.close();
            NDBG("File closed");

            DBG("Send event");
            //var data[1];
            //data[0] = var(uploadingFile.name());
            sendEvent(UploadDone);
            isUploading = false;
        }
        else
        {
            NDBG("Upload finish ERROR");
            isUploading = false;
        }
    }
    else if (upload.status == UPLOAD_FILE_ABORTED)
    {
        NDBG("ABOORT !!!!!!!!!!");
        uploadingFile.close();
        sendEvent(UploadCanceled);
        isUploading = false;
    }
}

void WebServerComponent::returnOK()
{
    server.send(200, "text/plain", "ok");
}

void WebServerComponent::returnFail(String msg)
{
    NDBG("Failed here");
    server.send(500, "text/plain", msg + "\r\n");
}

void WebServerComponent::handleNotFound()
{
    NDBG("Not found here");
    server.send(404, "text/plain", "[notfound]");
}

void WebServerComponent::handleQueryData()
{
    DynamicJsonDocument doc(32000);
    JsonObject o = doc.to<JsonObject>();
    RootComponent::instance->fillOSCQueryData(o);

    String jStr;
    serializeJson(doc, jStr);
    server.send(200, "application/json", jStr);
}

void WebServerComponent::handleSettings()
{
    bool configOnly = server.hasArg("configOnly") ? (bool)server.arg("configOnly").toInt() : false;

    String jStr;
    DynamicJsonDocument doc(32000);
    JsonObject o = doc.to<JsonObject>();
    RootComponent::instance->fillSettingsData(o, configOnly);
    serializeJson(doc, jStr);
    server.send(200, "application/json", jStr);
}