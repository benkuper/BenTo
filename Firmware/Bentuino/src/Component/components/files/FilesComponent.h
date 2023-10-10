#pragma once

DeclareComponentSingleton(Files, "files", )

bool initInternal(JsonObject o) override;
bool initInternalMemory();

SPIClass spiSD;
bool useInternalMemory;
DeclareIntParam(sdEnPin, FILES_DEFAULT_SD_EN);
DeclareBoolParam(sdEnVal, FILES_DEFAULT_SD_POWER_VALUE);
DeclareIntParam(sdSCK, FILES_DEFAULT_SD_SCK);
DeclareIntParam(sdMiso, FILES_DEFAULT_SD_MISO);
DeclareIntParam(sdMosi, FILES_DEFAULT_SD_MOSI);
DeclareIntParam(sdCS, FILES_DEFAULT_SD_CS);
DeclareIntParam(sdSpeed, FILES_DEFAULT_SDSPEED);


File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
void deleteFileIfExists(String path);
String listDir(const char *dirname, uint8_t levels);

bool handleCommandInternal(const String &command, var *data, int numData) override;

DeclareComponentEventTypes(UploadStart, UploadProgress, UploadComplete, UploadCancel, FileList);
DeclareComponentEventNames("uploadStart", "uploadProgress", "uploadComplete", "uploadCancel", "list");

EndDeclareComponent