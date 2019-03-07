#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <SPI.h>
#include <SD.h>

#define SDSPEED 27000000

#define SCK_PIN 33
#define MISO_PIN 25
#define MOSI_PIN 27
#define CS_PIN 2 //not using 13 because button_pin is on it

class FileManager
{
public:
   static bool sdIsDetected;

   static SPIClass spiSD;

   FileManager()
   {
   }

   void init()
   {
      spiSD.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);//SCK,MISO,MOSI,ss
      
      if(SD.begin( CS_PIN, spiSD, SDSPEED))
      {
        DBG("SD Card initialized.");
         sdIsDetected = true;
      }else{
        DBG("SD Card Initialization failed.");
      }
   }

   static File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true)
   {
      if(forWriting && deleteIfExists) deleteFileIfExists(fileName);
      
      if(!fileName.startsWith("/")) fileName = "/"+fileName;
      File f = SD.open(fileName.c_str(), forWriting?FILE_WRITE:FILE_READ);
      DBG("Open file : "+String(f.name()));
      return f;
   }

   static void deleteFileIfExists(String path)
   {
     if(!sdIsDetected) return;

      if (SD.exists(path.c_str()))
      {
         SD.remove(path.c_str());
         DBG("Removed file "+path);
      }
   }
/*
  }

  File dataFile = SD.open(path.c_str());
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile) {
    return false;
  }

  if (server.hasArg("download")) {
    dataType = "application/octet-stream";
  }

  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    DBG("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}
*/

};

SPIClass FileManager:: spiSD(HSPI);
bool FileManager::sdIsDetected = false;
#endif
