#pragma once

#include "../common/Common.h"

#ifdef HAS_FILES
#ifdef ESP32

#ifdef USE_SD_MMC
#include "SD_MMC.h"
#include "FS.h"
#else
#include <SPI.h>
#include "../../lib/SD/src/SD.h" //really weird
#endif


#include <WebServer.h>
#ifdef FILES_USE_INTERNAL_MEMORY
#include <SPIFFS.h>
#endif
#elif defined ESP8266
#include <ESP8266WebServer.h>
#endif

class FileEvent
{
public:
    enum Type
    {
        UploadStart,
        UploadProgress,
        UploadComplete,
        UploadCancel,
        FileList, 
        TYPES_MAX
    };
    static const String eventNames[TYPES_MAX];

    FileEvent(Type t, var data) : type(t), data(data) {}

    Type type;
    var data;
};

class FileManager : public Component, public EventBroadcaster<FileEvent>
{
public:
    FileManager();
    ~FileManager() {}


#ifdef FILES_USE_INTERNAL_MEMORY

#else
#ifndef USE_SD_MMC
    static SPIClass spiSD;
#endif
#endif

    File uploadingFile;

    static bool sdIsDetected;
    bool serverIsEnabled;
    int uploadedBytes;
    bool isUploading;
    
#ifdef ESP32
    WebServer server;
#elif defined ESP8266
    ESP8266WebServer server;
#endif

    void init();
    void update();

    //File manipulation
    static File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true);
    static void deleteFileIfExists(String path);

    static String listDir(const char *dirname, uint8_t levels);


    //Server handling
    void initServer();
    void closeServer();
    void handleFileUpload();
    void returnOK();
    void returnFail(String msg);
    void handleNotFound();

    // SD card enable
    void setSDEnabled(bool enable);

    bool handleCommand(String command, var *data, int numData) override;
};

#endif