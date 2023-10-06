#pragma once

DeclareComponentSingleton(Files, "files", );

bool initInternal(JsonObject o) override;
bool initInternalMemory();

SPIClass spiSD;
bool useInternalMemory;
Parameter sdEnPin{ "sdEnPin", FILES_DEFAULT_SD_EN, var(), var(), true};
Parameter sdEnVal{ "sdEnVal", FILES_DEFAULT_SD_POWER_VALUE, var(), var(), true};
Parameter sdSCK{ "sdSCK", FILES_DEFAULT_SD_SCK, var(), var(), true};
Parameter sdMiso{ "sdMiso", FILES_DEFAULT_SD_MISO, var(), var(), true};
Parameter sdMosi{ "sdMosi", FILES_DEFAULT_SD_MOSI, var(), var(), true};
Parameter sdCS{ "sdCS", FILES_DEFAULT_SD_CS, var(), var(), true};
Parameter sdSpeed{ "sdSpeed", FILES_DEFAULT_SDSPEED, var(), var(), true};

File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
void deleteFileIfExists(String path);
String listDir(const char *dirname, uint8_t levels);

bool handleCommandInternal(const String &command, var *data, int numData) override;

DeclareComponentEventTypes(UploadStart, UploadProgress, UploadComplete, UploadCancel, FileList);
DeclareComponentEventNames("uploadStart", "uploadProgress", "uploadComplete", "uploadCancel", "list");

EndDeclareComponent