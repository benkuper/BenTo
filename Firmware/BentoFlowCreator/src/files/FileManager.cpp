#include "FileManager.h"

SPIClass FileManager::spiSD(HSPI);
bool FileManager::sdIsDetected = false;

FileManager::FileManager() : Component("files"),
                             isUploading(false),
                             serverIsEnabled(false)
{
    server.onNotFound(std::bind(&FileManager::handleNotFound, this));
    server.on("/upload", HTTP_POST, std::bind(&FileManager::returnOK, this), std::bind(&FileManager::handleFileUpload, this));
}

void FileManager::init()
{
    pinMode(SD_EN, OUTPUT);
    digitalWrite(SD_EN, LOW);

    pinMode(SD_SCK, INPUT_PULLUP);
    pinMode(SD_MISO, INPUT_PULLUP);
    pinMode(SD_MOSI, INPUT_PULLUP);
    pinMode(SD_CS, INPUT_PULLUP);

    spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS); //SCK,MISO,MOSI,ss

    if (SD.begin(SD_CS, spiSD, SDSPEED))
    {
        NDBG("SD Card initialized.");
        listDir(SD, "/", 0);
        sdIsDetected = true;
    }
    else
    {
        NDBG("SD Card Initialization failed.");
    }


    initServer();
}

void FileManager::update()
{
    if(!serverIsEnabled) return;
    server.handleClient();
}

File FileManager::openFile(String fileName, bool forWriting, bool deleteIfExists)
{
    if (forWriting && deleteIfExists)
        deleteFileIfExists(fileName);

    if (!fileName.startsWith("/"))
        fileName = "/" + fileName;
    File f = SD.open(fileName.c_str(), forWriting ? FILE_WRITE : FILE_READ);
    DBG("Open file : " + String(f.name()));
    return f;
}

void FileManager::deleteFileIfExists(String path)
{
    if (!sdIsDetected)
        return;

    if (SD.exists(path.c_str()))
    {
        SD.remove(path.c_str());
        DBG("Removed file " + path);
    }
}

void FileManager::listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    File root = fs.open(dirname);
    if (!root)
    {
        DBG("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        DBG("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            DBG("  DIR : " + String(file.name()));
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            DBG("  FILE: " + String(file.name()));
            DBG("  SIZE: " + String(file.size()));
        }
        file = root.openNextFile();
    }
}

//SERVER
void FileManager::initServer()
{
    server.begin();
    NDBG("HTTP server started");
    serverIsEnabled = true;
}

void FileManager::closeServer()
{
    server.close();
    NDBG("HTTP server closed");
    serverIsEnabled = false;
}

void FileManager::handleFileUpload()
{
    if (server.uri() != "/upload") {
        return;
      }

      HTTPUpload& upload = server.upload();

      if (upload.status == UPLOAD_FILE_START) 
      {
        uploadedBytes = 0;
        //totalBytes = server.header("Content-Length").toInt();
        
        uploadingFile = FileManager::openFile(upload.filename, true, true);
        if(uploadingFile)
        {
          var data;
          data.type = 's';
          data.value.s = (char *)uploadingFile.name();
          sendEvent(FileEvent(FileEvent::UploadStart,data));
        }else
        {
          NDBG("ERROR WHEN CREATING THE FILE");
        }

        isUploading = true;
        
      } 
      else if (upload.status == UPLOAD_FILE_WRITE) 
      {
        if (uploadingFile) {
          if(uploadedBytes == 0 && upload.buf[0] == 13 && upload.buf[1] == 10)
          {
            NDBG("Remove new line nonsense");
            uploadingFile.write(upload.buf+2, upload.currentSize-2);
          }else
          {
            uploadingFile.write(upload.buf, upload.currentSize);
          }
          
          uploadedBytes += upload.currentSize;
          NDBG("Uploaded "+String(uploadedBytes));
          float p = uploadedBytes*1.0f/1000000;
          if(uploadedBytes % 8000 < 4000) 
          {
                var data;
                data.type = 'f';
                data.value.f = p;
                sendEvent(FileEvent(FileEvent::UploadProgress,data));
          }
        } 
      }
      else if (upload.status == UPLOAD_FILE_END) 
      {

        if (uploadingFile)
        {
          String n = uploadingFile.name();
          NDBG("Upload total size "+String(upload.totalSize)+" < > "+String(uploadingFile.size()));
          uploadingFile.close();
          
          var data;
          data.type = 's';
          data.value.s = (char *)uploadingFile.name();
          sendEvent(FileEvent(FileEvent::UploadComplete,data));
          isUploading = false;
        }else
        {
          NDBG("Upload finish ERROR");
          isUploading = false;
        }
        
      }else if(upload.status == UPLOAD_FILE_ABORTED)
      {
        NDBG("ABOORT !!!!!!!!!!");
        isUploading = false;
      }
}

void FileManager::returnOK()
{
    server.send(200, "text/plain", "ok");
}

void FileManager::returnFail(String msg)
{
    NDBG("Failed here");
    server.send(500, "text/plain", msg + "\r\n");
}

void FileManager::handleNotFound()
{
    NDBG("Not found here");
    server.send(404, "text/plain", "[notfound]");
}

bool FileManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "delete", numData, 1))
    {
        deleteFileIfExists(data[0].stringValue());
        return true;
    }
    else if (checkCommand(command, "delete", numData, 0))
    {
        if (numData > 0)
        {
            DBG("Deleting folder " + data[0].stringValue());
            SD.rmdir(data[0].stringValue());
        }
        else
        {
            DBG("Deleting all files");
            SD.rmdir("/");
        }

        return true;
    }

    return false;
}
