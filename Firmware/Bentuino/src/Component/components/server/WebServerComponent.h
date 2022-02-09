#pragma once
DeclareComponentSingleton(WebServer, "server",)

WebServer server;

bool isUploading;
int uploadedBytes;
File uploadingFile;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void onEnabledChanged() override;

void handleFileUpload();
void returnOK();
void returnFail(String msg);
void handleNotFound();

void handleQueryData();
void handleSettings();

DeclareComponentEventTypes (UploadStart, Uploading, UploadDone, UploadCanceled)
DeclareComponentEventNames("UploadStart","Uploading","UploadDone", "UploadCanceled")

EndDeclareComponent