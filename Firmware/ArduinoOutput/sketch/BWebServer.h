/*

#ifndef BWEBSERVER_H
#define BWEBSERVER_H

#include "FastLEDDefs.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "FileManager.h"

class BentoWebServer
{
public:
  BentoWebServer();
  ~BentoWebServer();

  WebServer server(80);

  void init()
  {
    server.onNotFound(handleNotFound);
    server.on("/upload", HTTP_POST, []() { returnOK(); }, handleFileUpload);
    server.onNotFound(handleNotFound);

   

     server.begin();
     DBG("HTTP server started");

     if (MDNS.begin(settings.deviceID.c_str())) {
      MDNS.addService("http", "tcp", 80);
      DBG("MDNS responder started, you can now connect to http://"+settings.deviceID+".local");
    }


    if (SD.begin(SS)) {
      DBG("SD Card initialized.");
      hasSD = true;
    }
  }

  void update()
  {
    server.handleClient();
  }

  void saveLightBlock()
  {
    
  }

  //Upload
  void handleFileUpload() {
    if (server.uri() != "/upload") {
      return;
    }
    
    HTTPUpload& upload = server.upload();
    
    if (upload.status == UPLOAD_FILE_START) {
      uploadFile = FileManager::openFile(upload.filename.c_str());
      DBG("Upload: START, filename: "+ String(upload.filename));

    } else if (upload.status == UPLOAD_FILE_WRITE) {
    
      if (uploadFile) { uploadFile.write(upload.buf, upload.currentSize);
      DBG("Upload: WRITE, Bytes: " + String(upload.currentSize);
    
    } else if (upload.status == UPLOAD_FILE_END) {

      if (uploadFile)  uploadFile.close();
      DBG("Upload: END, Size: "+String(DBG(upload.totalSize));
    
    }
  }


    void returnOK() {
      server.send(200, "text/plain", "");
    }

    void returnFail(String msg) {
      server.send(500, "text/plain", msg + "\r\n");
    }

    void returnNotFound() {
      server.send(404, "text/plain", "[notfound]");
    }
  
};

#endif

*/