#pragma once

class FilesComponent : public Component
{
public:
    FilesComponent() : Component(Type_Files) { instance = this; }
    ~FilesComponent() {}

    DeclareSingleton(FilesComponent);

    bool initInternal(JsonObject o) override;
    bool initInternalMemory();

    SPIClass spiSD;
    bool useInternalMemory;
    Parameter *sdEnPin;
    Parameter *sdEnVal;
    Parameter *sdSCK;
    Parameter *sdMiso;
    Parameter *sdMosi;
    Parameter *sdCS;
    Parameter *sdSpeed;

    File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
    void deleteFileIfExists(String path);
    String listDir(const char *dirname, uint8_t levels);

    bool handleCommandInternal(const String &command, var *data, int numData) override;

    DeclareComponentEventTypes(UploadStart, UploadProgress, UploadComplete, UploadCancel, FileList);
    DeclareComponentEventNames("uploadStart", "uploadProgress", "uploadComplete", "uploadCancel", "list");
};