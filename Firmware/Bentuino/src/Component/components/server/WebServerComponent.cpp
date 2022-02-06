ImplementSingleton(WebServerComponent)

    bool WebServerComponent::initInternal()
{
    server.onNotFound(std::bind(&WebServerComponent::handleNotFound, this));
    server.on("/upload", HTTP_POST, std::bind(&WebServerComponent::returnOK, this), std::bind(&WebServerComponent::handleFileUpload, this));
    server.on("/", HTTP_ANY, std::bind(&WebServerComponent::handleQueryData, this));

    enabled->set(false);

    return true;
}

void WebServerComponent::updateInternal()
{
    if(!enabled->boolValue()) return;
    server.handleClient();
}

void WebServerComponent::clearInternal()
{
}

// SERVER
void WebServerComponent::onEnabledChanged()
{
    if (enabled->boolValue())
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
    if (server.uri() != "/upload")
    {
        return;
    }

    HTTPUpload &upload = server.upload();

    if (upload.status == UPLOAD_FILE_START)
    {
        uploadedBytes = 0;
        // totalBytes = server.header("Content-Length").toInt();

        uploadingFile = FilesComponent::instance->openFile(upload.filename, true, true);
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

        if (uploadingFile)
        {
            String n = uploadingFile.name();
            NDBG("Upload total size " + String(upload.totalSize) + " < > " + String(uploadingFile.size()));
            uploadingFile.close();

            var data[1];
            data[0] = (char *)uploadingFile.name();
            sendEvent(UploadDone, data, 1);
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
    NDBG("Handle Query Data");
    
    DynamicJsonDocument doc(8000);
    JsonObject o = doc.to<JsonObject>();
    RootComponent::instance->fillJSONData(o);

    String jStr;
    serializeJson(doc, jStr);
    server.send(200, "application/json", jStr);
}
