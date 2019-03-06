
#ifndef BWEBSERVER_H
#define BWEBSERVER_H

#include <WebServer.h>
#include <ESPmDNS.h>

#include "FileManager.h"

class BentoWebServer
{
 public:
    BentoWebServer() {}
    ~BentoWebServer() {}

   static BentoWebServer * instance;
    
    static WebServer server;
    static File uploadingFile;

    void init()
    {
      instance = this;
      
      server.onNotFound(BentoWebServer::handleNotFound);
      server.on("/upload", HTTP_POST, []() {
        returnOK();
      },
      BentoWebServer::handleFileUpload
      );

      server.begin();
      DBG("HTTP server started");

      /* if (MDNS.begin(settings.deviceID.c_str())) {
        MDNS.addService("http", "tcp", 80);
        DBG("MDNS responder started, you can now connect to http://"+settings.deviceID+".local");
        }
      */

      addUploadStartCallback(&BentoWebServer::onUploadDefaultCallback);
      addUploadProgressCallback(&BentoWebServer::onUploadProgressDefaultCallback);
      addUploadFinishCallback(&BentoWebServer::onUploadDefaultCallback); 
    }

    void update()
    {
      server.handleClient();
    }

    //Upload
    static void handleFileUpload() {
      if (server.uri() != "/upload") {
        return;
      }

      HTTPUpload& upload = server.upload();

      if (upload.status == UPLOAD_FILE_START) {
        uploadingFile = FileManager::openFile(upload.filename.c_str());
        DBG("Upload: START, filename: " + String(upload.filename));
        instance->onUploadStartEvent();
        
      } else if (upload.status == UPLOAD_FILE_WRITE) {

        if (uploadingFile) {
          uploadingFile.write(upload.buf, upload.currentSize);
          DBG("Upload: WRITE, Bytes: " + String(upload.currentSize));
          float p = upload.currentSize*1.0f/1000000;
           instance->onUploadProgressEvent(p);
          
        } else if (upload.status == UPLOAD_FILE_END) {

          if (uploadingFile)  uploadingFile.close();
          DBG("Upload: END, Size: " + String(upload.totalSize));
          instance->onUploadFinishEvent();
        }
      }
    }

    static void returnOK() {
      server.send(200, "text/plain", "ok");
    }

    static void returnFail(String msg) {
      server.send(500, "text/plain", msg + "\r\n");
    }

    static void handleNotFound() {
      server.send(404, "text/plain", "[notfound]");
    }


    typedef void(*UploadProgressEvent)(float progress);
    void (*onUploadProgressEvent) (float progress);

    typedef void(*UploadEvent)();
    void (*onUploadStartEvent)();
    void (*onUploadFinishEvent)();

    void addUploadStartCallback (UploadEvent func) {  onUploadStartEvent = func;  }
    void addUploadFinishCallback (UploadEvent func) {  onUploadFinishEvent = func;  }
    void addUploadProgressCallback (UploadProgressEvent func) {  onUploadProgressEvent = func;  }
    
    static void onUploadDefaultCallback() { DBG("Upload start/finish  default callback"); }
    static void onUploadProgressDefaultCallback(float progress) { DBG("Upload progress : "+String(progress)); }
    

};

BentoWebServer * BentoWebServer::instance = NULL;
File BentoWebServer::uploadingFile;
WebServer BentoWebServer::server(80);
#endif
