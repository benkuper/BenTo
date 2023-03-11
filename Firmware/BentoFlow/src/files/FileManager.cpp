#include "FileManager.h"

const String FileEvent::eventNames[FileEvent::TYPES_MAX]{"uploadStart", "uploadProgress", "uploadComplete", "uploadCancel", "list"};
bool FileManager::sdIsDetected = false;

#ifdef HAS_FILES
#ifndef FILES_USE_INTERNAL_MEMORY
#ifndef USE_SD_MMC
SPIClass FileManager::spiSD(HSPI);
#endif
#endif
#endif

FileManager::FileManager() : Component("files"),
                             serverIsEnabled(false),
                             isUploading(false)
#ifdef HAS_FILES
                             ,
                             server(80)
#endif
{
#ifdef HAS_FILES
    server.onNotFound(std::bind(&FileManager::handleNotFound, this));
    server.on("/upload", HTTP_POST, std::bind(&FileManager::returnOK, this), std::bind(&FileManager::handleFileUpload, this));
#endif
}

void FileManager::init()
{
    if (sdIsDetected)
        return;

#ifdef HAS_FILES
    NDBG("Init");

#ifdef FILES_USE_INTERNAL_MEMORY

#ifdef ESP32
    if (SPIFFS.begin(true)) // Start the SPI Flash Files System
#else
    if (SPIFFS.begin())
#endif
    {
        sdIsDetected = true;
        NDBG("SPIFFS initialized.");
        listDir("/", 0);
    }
    else
    {
        NDBG("Error initializing SPIFFS");
    }

#else

#ifdef SD_EN
    pinMode(SD_EN, OUTPUT);
    digitalWrite(SD_EN, SD_POWER_VALUE);
#endif

#ifdef USE_SD_MMC
    if(SD_MMC.begin())
#else
    pinMode(SD_SCK, INPUT_PULLUP);
    pinMode(SD_MISO, INPUT_PULLUP);
    pinMode(SD_MOSI, INPUT_PULLUP);
    pinMode(SD_CS, INPUT_PULLUP);

    spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS); // SCK,MISO,MOSI,ss

    if (SD.begin(SD_CS, spiSD, SDSPEED))
#endif
    {
        NDBG("SD Card initialized.");
        listDir("/", 0);
        sdIsDetected = true;
    }
    else
    {
        NDBG("SD Card Initialization failed.");
    }
#endif // FILES_USE_INTERNAL_MEMORY

    initServer();

#endif // HAS_FILES
}

void FileManager::update()
{
#ifdef HAS_FILES
    if (!serverIsEnabled)
        return;
    server.handleClient();
#endif
}

#ifdef HAS_FILES
File FileManager::openFile(String fileName, bool forWriting, bool deleteIfExists)
{

    if (forWriting && deleteIfExists)
        deleteFileIfExists(fileName);

    if (!fileName.startsWith("/"))
        fileName = "/" + fileName;

#ifdef FILES_USE_INTERNAL_MEMORY
    File f = SPIFFS.open(fileName, forWriting ? "w" : "r"); // Open it
#else
#ifdef USE_SD_MMC
    File f = SD_MMC.open(fileName.c_str(), forWriting ? FILE_WRITE : FILE_READ);
#else
    File f = SD.open(fileName.c_str(), forWriting ? FILE_WRITE : FILE_READ);
#endif
#endif

    DBG("Open file : " + String(f.name()));
    return f;
    return File();
}

void FileManager::deleteFileIfExists(String path)
{
    if (!sdIsDetected)
        return;

#ifdef FILES_USE_INTERNAL_MEMORY
    DBG("Delete if exists : " + path);
    if (SPIFFS.exists(path))
    {
        DBG("File exists, remove : " + path);
        SPIFFS.remove(path);
    }
#else
#ifdef USE_SD_MMC
    if (SD_MMC.exists(path.c_str()))
#else
    if (SD.exists(path.c_str()))
#endif
    {
#ifdef USE_SD_MMC
        SD_MMC.remove(path.c_str());
#else
        SD.remove(path.c_str());
#endif
        DBG("Removed file " + path);
    }
#endif
}

String FileManager::listDir(const char *dirname, uint8_t levels)
{
    String result = "";

#ifdef FILES_USE_INTERNAL_MEMORY
    File root = SPIFFS.open("/", "r");
#else
#ifdef USE_SD_MMC
    File root = SD_MMC.open(dirname);
#else
    File root = SD.open(dirname);
#endif
#endif

    if (!root)
    {
        DBG("Failed to open directory");
        return result;
    }

    if (!root.isDirectory())
    {
        DBG("Not a directory");
        return result;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            DBG("  DIR : " + String(file.name()));
            if (levels)
            {
                result += listDir(file.name(), levels - 1);
            }
        }
        else
        {
            DBG("  FILE: " + String(file.name()));
            result += String(file.name()) + ",";
            DBG("  SIZE: " + String(file.size()));
        }
        file = root.openNextFile();
    }

    return result;
}
#endif // HAS_FILES

// SERVER
void FileManager::initServer()
{
#ifdef HAS_FILES
    server.begin();
    NDBG("HTTP server started");
    serverIsEnabled = true;
#endif
}

void FileManager::closeServer()
{
#ifdef HAS_FILES
    server.close();
    NDBG("HTTP server closed");
    serverIsEnabled = false;
#endif
}

void FileManager::handleFileUpload()
{
#ifdef HAS_FILES
    if (server.uri() != "/upload")
    {
        return;
    }

    HTTPUpload &upload = server.upload();

    if (upload.status == UPLOAD_FILE_START)
    {
        uploadedBytes = 0;
        // totalBytes = server.header("Content-Length").toInt();

        uploadingFile = openFile(upload.filename, true, true);
        if (uploadingFile)
        {
            var data;
            data.type = 's';
            data.value.s = (char *)uploadingFile.name();
            sendEvent(FileEvent(FileEvent::UploadStart, data));
        }
        else
        {
            NDBG("ERROR WHEN CREATING THE FILE");
        }

        isUploading = true;
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (uploadingFile)
        {
            if (uploadedBytes == 0 && upload.buf[0] == 13 && upload.buf[1] == 10)
            {
                NDBG("Remove new line nonsense");
                uploadingFile.write(upload.buf + 2, upload.currentSize - 2);
            }
            else
            {
                uploadingFile.write(upload.buf, upload.currentSize);
            }

            uploadedBytes += upload.currentSize;
            float p = uploadedBytes * 1.0f / 1000000;
            if (uploadedBytes % 8000 < 4000)
            {
                var data;
                data.type = 'f';
                data.value.f = p;
                sendEvent(FileEvent(FileEvent::UploadProgress, data));
            }
        }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {

        if (uploadingFile)
        {
            String n = uploadingFile.name();
            NDBG("Upload total size " + String(upload.totalSize) + " < > " + String(uploadingFile.size()));
            uploadingFile.close();

            var data;
            data.type = 's';
            data.value.s = (char *)uploadingFile.name();
            sendEvent(FileEvent(FileEvent::UploadComplete, data));
            isUploading = false;
        }
        else
        {
            NDBG("Upload finish ERROR");
            isUploading = false;
        }
    }
    else if (upload.status == UPLOAD_FILE_ABORTED)
    {
        NDBG("ABOORT !!!!!!!!!!");
        uploadingFile.close();
        isUploading = false;
    }
#endif
}

void FileManager::returnOK()
{
#ifdef HAS_FILES
    server.send(200, "text/plain", "ok");
#endif
}

void FileManager::returnFail(String msg)
{
#ifdef HAS_FILES
    NDBG("Failed here");
    server.send(500, "text/plain", msg + "\r\n");
#endif
}

void FileManager::handleNotFound()
{
#ifdef HAS_FILES
    NDBG("Not found here");
    server.send(404, "text/plain", "[notfound]");
#endif
}

void FileManager::setSDEnabled(bool enable)
{
#ifdef SD_EN
    if (enable)
    {
        digitalWrite(SD_EN, LOW);
        NDBG("Enabling SD card");
    }
    else
    {
        digitalWrite(SD_EN, HIGH);
        NDBG("Disabled SD card");
    }
#endif
}

bool FileManager::handleCommand(String command, var *data, int numData)
{
#ifdef HAS_FILES

    if (checkCommand(command, "delete", numData, 1))
    {
        deleteFileIfExists(data[0].stringValue());
        return true;
    }
    else if (checkCommand(command, "enable", numData, 1))
    {
        setSDEnabled(data[0].intValue() == 1);
        return true;
    }
    else if (checkCommand(command, "deleteFolder", numData, 0))
    {
        if (numData > 0)
        {
            DBG("Deleting folder " + data[0].stringValue());
#ifdef FILES_USE_INTERNAL_MEMORY
            SPIFFS.rmdir(data[0].stringValue());
#else
#ifdef USE_SD_MMC
            SD_MMC.rmdir(data[0].stringValue());
#else
            SD.rmdir(data[0].stringValue());
#endif
#endif
        }
        else
        {
            DBG("Deleting all files");
#ifdef FILES_USE_INTERNAL_MEMORY
            SPIFFS.rmdir("/");
#else
#ifdef USE_SD_MMC
            SD_MMC.rmdir("/");
#else
            SD.rmdir("/");
#endif
#endif
        }

        return true;
    }
    else if (checkCommand(command, "list", numData, 0))
    {
        var data;
        data.type = 's';
        data.value.s = (char *)listDir("/", 0).c_str();

        sendEvent(FileEvent(FileEvent::FileList, data));
        return true;
    }
#endif // HAS_FILES

    return false;
}