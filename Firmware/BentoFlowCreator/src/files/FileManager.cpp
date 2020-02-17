#include "FileManager.h"

SPIClass FileManager:: spiSD(HSPI);
bool FileManager::sdIsDetected = false;

FileManager::FileManager() :
    Component("files")
{
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
}

void FileManager::update()
{
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

bool FileManager::handleCommand(String command, var *data, int numData)
{
    if(checkCommand(command, "delete", numData, 1))
    {
      deleteFileIfExists(data[0].stringValue());
      return true;
      
    }else if(checkCommand(command, "delete", numData, 0))
    {
      if(numData > 0)
      {
        DBG("Deleting folder "+data[0].stringValue());
        SD.rmdir(data[0].stringValue());
      }else
      {
        DBG("Deleting all files");
        SD.rmdir("/");
      }

      return true;
    }
    
    return false;
}
