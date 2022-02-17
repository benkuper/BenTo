#pragma once
class WebServerComponent : public Component
{
public:
    WebServerComponent() : Component(Type_Server) { instance = this; }
    ~WebServerComponent() {}

    DeclareSingleton(WebServerComponent);

    WebServer server;
    StaticJsonDocument<16000> doc; // not in ram then

    bool isUploading;
    int uploadedBytes;
    File uploadingFile;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    void onEnabledChanged() override;

    void setupConnection();

    void handleFileUpload();
    void returnOK();
    void returnFail(String msg);
    void handleNotFound();

    void handleQueryData();
    void handleSettings();

    DeclareComponentEventTypes(UploadStart, Uploading, UploadDone, UploadCanceled);
    DeclareComponentEventNames("UploadStart", "Uploading", "UploadDone", "UploadCanceled");
};