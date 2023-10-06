#pragma once

DeclareComponentSingleton(Files, "files", );

bool initInternal(JsonObject o) override;
bool initInternalMemory();

SPIClass spiSD;
bool useInternalMemory;
DeclareConfigParameter(sdEnPin, FILES_DEFAULT_SD_EN);
DeclareConfigParameter(sdEnVal, FILES_DEFAULT_SD_POWER_VALUE);
DeclareConfigParameter(sdSCK, FILES_DEFAULT_SD_SCK);
DeclareConfigParameter(sdMiso, FILES_DEFAULT_SD_MISO);
DeclareConfigParameter(sdMosi, FILES_DEFAULT_SD_MOSI);
DeclareConfigParameter(sdCS, FILES_DEFAULT_SD_CS);
DeclareConfigParameter(sdSpeed, FILES_DEFAULT_SDSPEED);

File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
void deleteFileIfExists(String path);
String listDir(const char *dirname, uint8_t levels);

bool handleCommandInternal(const String &command, var *data, int numData) override;

DeclareComponentEventTypes(UploadStart, UploadProgress, UploadComplete, UploadCancel, FileList);
DeclareComponentEventNames("uploadStart", "uploadProgress", "uploadComplete", "uploadCancel", "list");

EndDeclareComponent