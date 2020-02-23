#pragma once

#include "../common/Common.h"

#include <SPI.h>
#include <SD.h>
#include <ESPmDNS.h>
#include <WebServer.h>

class FileEvent
{
public:
    enum Type { UploadStart, UploadProgress, UploadComplete, UploadCancel };
    FileEvent(Type t, var data) : type(t), data(data) {}

    Type type;
    var data;
};

class FileManager : public Component, public EventBroadcaster<FileEvent> {
public:
    FileManager();
    ~FileManager() {}

    static SPIClass spiSD;
    static bool sdIsDetected;

    bool serverIsEnabled;
    File uploadingFile;
    WebServer server;
    int uploadedBytes;
    bool isUploading;

    void init();
    void update();

    //File manipulation
    static File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
    static void deleteFileIfExists(String path);
    static void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

    //Server handling
    void initServer();
    void closeServer();
    void handleFileUpload();
    void returnOK();
    void returnFail(String msg);
    void handleNotFound();

    bool handleCommand(String command, var *data, int numData) override;
};
