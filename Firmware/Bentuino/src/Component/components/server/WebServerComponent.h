#pragma once
DeclareComponentSingleton(WebServer, "server",)

AsyncWebServer server = AsyncWebServer(80);
AsyncWebSocket ws = AsyncWebSocket("/ws");

bool isUploading;
int uploadedBytes;
File uploadingFile;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void onEnabledChanged() override;

void setupConnection();

void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

// void returnOK();
// void returnFail(String msg);
// void handleNotFound();

// void handleQueryData();
// void handleSettings();

DeclareComponentEventTypes (UploadStart, Uploading, UploadDone, UploadCanceled)
DeclareComponentEventNames("UploadStart","Uploading","UploadDone", "UploadCanceled")

EndDeclareComponent