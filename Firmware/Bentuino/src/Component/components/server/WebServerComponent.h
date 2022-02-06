#pragma once
DeclareComponentSingleton(WebServer, "server",)

WebServer server;

bool isUploading;
int uploadedBytes;
File uploadingFile;

bool initInternal() override;
void updateInternal() override;
void clearInternal() override;

void onEnabledChanged() override;

void handleFileUpload();
void returnOK();
void returnFail(String msg);
void handleNotFound();

void handleQueryData();

DeclareComponentEventTypes (UploadStart, Uploading, UploadDone, UploadCanceled)
DeclareComponentEventNames("UploadStart","Uploading","UploadDone", "UploadCanceled")

EndDeclareComponent