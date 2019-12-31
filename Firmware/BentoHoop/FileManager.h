#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <SPI.h>
#include <SD.h>

#define SDSPEED 27000000

#define SCK_PIN 33
#define MISO_PIN 27
#define MOSI_PIN 25
#define CS_PIN 2 //not using 13 because button_pin is on it
#define SD_ENABLE 32

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
      DBG("Init Filemanager");
      
      pinMode(SD_ENABLE, OUTPUT);
      digitalWrite(SD_ENABLE, LOW);

      pinMode(SCK_PIN, INPUT_PULLUP);
      pinMode(MISO_PIN, INPUT_PULLUP);
      pinMode(MOSI_PIN, INPUT_PULLUP);
      pinMode(CS_PIN, INPUT_PULLUP);
      
      spiSD.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);//SCK,MISO,MOSI,ss
      
      if(SD.begin( CS_PIN, spiSD, SDSPEED))
      {
        DBG("SD Card initialized.");
        listDir(SD, "/", 0);
        sdIsDetected = true;
      }else{
        DBG("SD Card Initialization failed.");
      }
   }

   static File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true)
   {
      listDir(SD, "/", 0);
      
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

   static void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
      DBG("Listing directory: " + String(dirname));
  
      File root = fs.open(dirname);
      if(!root){
          DBG("Failed to open directory");
          return;
      }
      if(!root.isDirectory()){
          DBG("Not a directory");
          return;
      }
  
      File file = root.openNextFile();
      while(file){
          if(file.isDirectory()){
              DBG("  DIR : " + String(file.name()));
              if(levels){
                  listDir(fs, file.name(), levels -1);
              }
          } else {
              DBG("  FILE: " + String(file.name()));
              DBG("  SIZE: " + String(file.size()));
          }
          file = root.openNextFile();
      }
  }

#if USE_OSC
  boolean handleMessage(OSCMessage &msg)
  {
    DBG("File Manager parse message");
    
    int offset = msg.match("/file");
    if(offset == 0) return false;
    
    if(msg.match("/delete", offset))
    {
      char filename[32];
      msg.getString(0,filename, 32);
      deleteFileIfExists(String(filename));
    }else if(msg.match("/deleteAll", offset))
    {

      if(msg.size() > 0)
      {
         char folderData[32];
         msg.getString(0,folderData, 32);
         DBG("Deleting folder "+String(folderData));
         SD.rmdir(folderData);
      }else
      {
        DBG("Deleting all files");
        SD.rmdir("/");
      }
    }else
    {
      DBG("Message not handled");
    }
    
    return true;
  }
#endif

};

SPIClass FileManager:: spiSD(HSPI);
bool FileManager::sdIsDetected = false;
#endif
