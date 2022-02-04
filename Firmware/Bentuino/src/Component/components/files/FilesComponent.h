#pragma once

DeclareComponentSingleton(Files, "files", );

bool initInternal() override;

SPIClass spiSD;
bool useInternalMemory;
int sdEnPin;
int sdEnVal;

File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
void deleteFileIfExists(String path);
String listDir(const char *dirname, uint8_t levels);

bool handleCommandInternal(const String &command, var *data, int numData) override;

DeclareComponentEventTypes(UploadStart, UploadProgress, UploadComplete, UploadCancel, FileList);
DeclareComponentEventNames("uploadStart", "uploadProgress", "uploadComplete", "uploadCancel", "list");

EndDeclareComponent