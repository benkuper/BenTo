#pragma once

DeclareComponentSingleton(Files, "files", );

bool initInternal(JsonObject o) override;
bool initInternalMemory();

SPIClass spiSD;
bool useInternalMemory;
Parameter sdEnPin{ "sdEnPin", 0, var(), var(), true};
Parameter sdEnVal{ "sdEnVal", 0, var(), var(), true};
Parameter sdSCK{ "sdSCK", 0, var(), var(), true};
Parameter sdMiso{ "sdMiso", 0, var(), var(), true};
Parameter sdMosi{ "sdMosi", 0, var(), var(), true};
Parameter sdCS{ "sdCS", 0, var(), var(), true};
Parameter sdSpeed{ "sdSpeed", 27000000, var(), var(), true};

File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
void deleteFileIfExists(String path);
String listDir(const char *dirname, uint8_t levels);

bool handleCommandInternal(const String &command, var *data, int numData) override;

DeclareComponentEventTypes(UploadStart, UploadProgress, UploadComplete, UploadCancel, FileList);
DeclareComponentEventNames("uploadStart", "uploadProgress", "uploadComplete", "uploadCancel", "list");

EndDeclareComponent