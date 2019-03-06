#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <SPI.h>
#include <SD.h>

#define SDSPEED 27000000

#define SCK_PIN 33
#define MISO_PIN 25
#define MOSI_PIN 27
#define CS_PIN 13

class FileManager
{
 public:
   static bool sdIsDetected;

   SPIClass spiSD(HSPI);

   FileManager()
   {
   }

   void init()
   {
      spiSD.begin(SCK_MIN, MISO_PIN, MOSI_PIN, CS_PIN);//SCK,MISO,MOSI,ss
      if(SD.begin( CS_PIN, spiSD, SDSPEED))
      {
        DBG("SD Card initialized.");
         sdIsDetected = true;
      }else{
        DBG("SD Card Initialization failed.");
      }
   }

   static File openFile(String fileName)
   {
      return SD.open(fileName.c_str(), FILE_WRITE);
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


bool FileManager::sdIsDetected = false;
#endif
