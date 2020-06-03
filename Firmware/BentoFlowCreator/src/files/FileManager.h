#pragma once

#include "../common/Common.h"

#ifdef HAS_FILES
#ifdef ESP32
#include <SPI.h>
#include <SD.h>
#include <WebServer.h>
#ifdef FILES_USE_INTERNAL_MEMORY
#include <SPIFFS.h>
#endif
#elif defined ESP8266
#include <ESP8266WebServer.h>
#endif

#endif

class FileEvent
{
public:
    enum Type
    {
        UploadStart,
        UploadProgress,
        UploadComplete,
        UploadCancel
    };
    FileEvent(Type t, var data) : type(t), data(data) {}

    Type type;
    var data;
};

class FileManager : public Component, public EventBroadcaster<FileEvent>
{
public:
    FileManager();
    ~FileManager() {}

#ifdef HAS_FILES

#ifdef FILES_USE_INTERNAL_MEMORY

#else
    static SPIClass spiSD;
#endif

    File uploadingFile;

#ifdef ESP32
    WebServer server;
#elif defined ESP8266
    ESP8266WebServer server;
#endif
#endif

    static bool sdIsDetected;
    bool serverIsEnabled;
    int uploadedBytes;
    bool isUploading;

    void init();
    void update();

    //File manipulation
#ifdef HAS_FILES
    static File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
    static void deleteFileIfExists(String path);

    static void listDir(const char *dirname, uint8_t levels);

#endif

    //Server handling
    void initServer();
    void closeServer();
    void handleFileUpload();
    void returnOK();
    void returnFail(String msg);
    void handleNotFound();

    bool handleCommand(String command, var *data, int numData) override;
};
