
#ifndef BWEBSERVER_H
#define BWEBSERVER_H

#include <WebServer.h>
#include <ESPmDNS.h>

#include "FileManager.h"

class BentoWebServer
{
 public:
   
    static BentoWebServer * instance;
    static File uploadingFile;
    static WebServer server;
    static int uploadedBytes;
    
    static bool isUploading;
    static int uploadIndex;

    bool isEnabled;
    
     BentoWebServer()
     {
        instance = this;

        server.onNotFound(BentoWebServer::handleNotFound);
        server.on("/upload", HTTP_POST, []() { returnOK(); }, BentoWebServer::handleFileUpload);
     }
    
    void init()
    {
      isUploading = false;
      isEnabled = false;
      
      addUploadStartCallback(&BentoWebServer::onUploadDefaultCallback);
      addUploadProgressCallback(&BentoWebServer::onUploadProgressDefaultCallback);
      addUploadFinishCallback(&BentoWebServer::onUploadDefaultCallback); 

      initServer();
      
    }

    void initServer()
    {
      server.begin();
      DBG("HTTP server started");
      isEnabled = true;
    }

    void closeServer()
    {
      server.close();
      DBG("HTTP server closed");
      isEnabled = false;
    }

    void update()
    {
      if(!isEnabled) return;
      
      server.handleClient();
    }

    //Upload
    static void handleFileUpload() {
      if (server.uri() != "/upload") {
        return;
      }

      HTTPUpload& upload = server.upload();

      if (upload.status == UPLOAD_FILE_START) 
      {
        uploadedBytes = 0;
        //totalBytes = server.header("Content-Length").toInt();
        uploadIndex = 0;
        
        uploadingFile = FileManager::openFile(upload.filename, true, true);
        if(uploadingFile)
        {
          instance->onUploadStartEvent(uploadingFile.name());
        }else
        {
          DBG("ERROR WHEN CREATING THE FILE");
        }

        isUploading = true;
        
      } 
      else if (upload.status == UPLOAD_FILE_WRITE) 
      {
        if (uploadingFile) {
          if(uploadedBytes == 0 && upload.buf[0] == 13 && upload.buf[1] == 10)
          {
            DBG("Remove new line nonsense");
            uploadingFile.write(upload.buf+2, upload.currentSize-2);
          }else
          {
            uploadingFile.write(upload.buf, upload.currentSize);
          }
          
          uploadedBytes += upload.currentSize;
          DBG("Uploaded "+String(uploadedBytes));
          float p = uploadedBytes*1.0f/1000000;
          if(uploadedBytes % 8000 < 4000) instance->onUploadProgressEvent(p);
        } 
      }
      else if (upload.status == UPLOAD_FILE_END) 
      {

        if (uploadingFile)
        {
          String n = uploadingFile.name();
          DBG("Upload total size "+String(upload.totalSize)+" < > "+String(uploadingFile.size()));
          uploadingFile.close();
          instance->onUploadFinishEvent(n);
          isUploading = false;
        }else
        {
          DBG("Upload finish ERROR");
          isUploading = false;
        }
        
      }else if(upload.status == UPLOAD_FILE_ABORTED)
      {
        DBG("ABOORT !!!!!!!!!!");
        isUploading = false;
      }
    }

    static void returnOK() {
      server.send(200, "text/plain", "ok");
    }

    static void returnFail(String msg) {
      DBG("Failed here");
      server.send(500, "text/plain", msg + "\r\n");
    }

    static void handleNotFound() {
      DBG("Not found here");
      server.send(404, "text/plain", "[notfound]");
    }


    typedef void(*UploadProgressEvent)(float progress);
    void (*onUploadProgressEvent) (float progress);

    typedef void(*UploadEvent)(const String &fileName);
    void (*onUploadStartEvent)(const String &fileName);
    void (*onUploadFinishEvent)(const String &fileName);

    void addUploadStartCallback (UploadEvent func) {  onUploadStartEvent = func;  }
    void addUploadFinishCallback (UploadEvent func) {  onUploadFinishEvent = func;  }
    void addUploadProgressCallback (UploadProgressEvent func) {  onUploadProgressEvent = func;  }
    
    static void onUploadDefaultCallback(const String &fileName) { DBG("Upload start/finish "+String(fileName)); }
    static void onUploadProgressDefaultCallback(float progress) { DBG("Upload progress : "+String(progress)); }
    

};

BentoWebServer * BentoWebServer::instance = NULL;
WebServer BentoWebServer::server(80);
File BentoWebServer::uploadingFile;
int BentoWebServer::uploadedBytes = 0;
bool BentoWebServer::isUploading = false;
int BentoWebServer::uploadIndex = 0;
#endif
