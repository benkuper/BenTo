#pragma once

DeclareComponentSingleton(Files, "files", )

    bool initInternal(JsonObject o) override;
bool initInternalMemory();

#ifdef FILES_TYPE_MMC
#define FS_TYPE SD_MMC
#else
#define FS_TYPE SD
SPIClass spiSD;
#endif

static fs::FS &fs;

bool useInternalMemory;
#ifdef FILES_TYPE_MMC
#else
DeclareIntParam(sdEnPin, FILES_DEFAULT_SD_EN);
DeclareBoolParam(sdEnVal, FILES_DEFAULT_SD_POWER_VALUE);
DeclareIntParam(sdSCK, FILES_DEFAULT_SD_SCK);
DeclareIntParam(sdMiso, FILES_DEFAULT_SD_MISO);
DeclareIntParam(sdMosi, FILES_DEFAULT_SD_MOSI);
DeclareIntParam(sdCS, FILES_DEFAULT_SD_CS);
DeclareIntParam(sdSpeed, FILES_DEFAULT_SDSPEED);
#endif

File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
bool deleteFolder(String path);
void deleteFileIfExists(String path);
String listDir(const char *dirname, uint8_t levels);

bool handleCommandInternal(const String &command, var *data, int numData) override;

DeclareComponentEventTypes(UploadStart, UploadProgress, UploadComplete, UploadCancel, FileList);
DeclareComponentEventNames("uploadStart", "uploadProgress", "uploadComplete", "uploadCancel", "list");

#ifdef FILES_TYPE_SD
HandleSetParamInternalStart
    CheckAndSetParam(sdEnPin);
CheckAndSetParam(sdEnVal);
CheckAndSetParam(sdSCK);
CheckAndSetParam(sdMiso);
CheckAndSetParam(sdMosi);
CheckAndSetParam(sdCS);
CheckAndSetParam(sdSpeed);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(sdEnPin);
FillSettingsParam(sdEnVal);
FillSettingsParam(sdSCK);
FillSettingsParam(sdMiso);
FillSettingsParam(sdMosi);
FillSettingsParam(sdCS);
FillSettingsParam(sdSpeed);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryIntParam(sdEnPin);
FillOSCQueryBoolParam(sdEnVal);
FillOSCQueryIntParam(sdSCK);
FillOSCQueryIntParam(sdMiso);
FillOSCQueryIntParam(sdMosi);
FillOSCQueryIntParam(sdCS);
FillOSCQueryIntParam(sdSpeed);
FillOSCQueryInternalEnd
#endif

    EndDeclareComponent
