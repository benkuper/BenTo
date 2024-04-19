#pragma once
DeclareComponentSingleton(WebServer, "server", )

    class WSPrint : public Print
{
public:
    uint8_t data[128]; // max 128 chars, should be enough
    int index = 0;
    size_t write(uint8_t c) override
    {
        if (index < 128)
            data[index++] = c;
        return 1;
    }
    void flush() { index = 0; };
};

DeclareBoolParam(sendFeedback, true);

AsyncWebServer server = AsyncWebServer(80);

#if USE_ASYNC_WEBSOCKET
AsyncWebSocket ws = AsyncWebSocket("/");
#else
WebSocketsServer ws = WebSocketsServer(81);
#endif

WSPrint wsPrint;

bool isUploading;
int uploadedBytes;
File uploadingFile;

String tmpExcludeParam = ""; // to change with client exclude when AsyncWebServer implements it

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void onEnabledChanged() override;

void setupConnection();

void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

#if USE_ASYNC_WEBSOSCKET
void onAsyncWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                    void *arg, uint8_t *data, size_t len);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
#else
void onWSEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
#endif

// void sendParameterFeedback(Component *c, Parameter *p);
void sendParamFeedback(Component *c, String pName, var *data, int numData);

// void returnOK();
// void returnFail(String msg);
// void handleNotFound();

// void handleQueryData();
// void handleSettings();

DeclareComponentEventTypes(UploadStart, Uploading, UploadDone, UploadCanceled);
DeclareComponentEventNames("UploadStart", "Uploading", "UploadDone", "UploadCanceled");

HandleSetParamInternalStart
    CheckAndSetParam(sendFeedback);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(sendFeedback);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryBoolParam(sendFeedback);
FillOSCQueryInternalEnd;

EndDeclareComponent