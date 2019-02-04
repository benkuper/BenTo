/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.
 
 
 BEGIN_JUCE_MODULE_DECLARATION
 
 ID:               juce_mongoose
 vendor:           open
 version:          5.3.1
 name:             JUCE Mongoose Webserver
 description:      Wrapper for the Mongoose webserver C++ bindings
 website:          https://github.com/cesanta/mongoose
 
 dependencies:     juce_core
 
 END_JUCE_MODULE_DECLARATION
 
 *******************************************************************************/


#pragma once
#define JUCE_MONGOOSE_H_INCLUDED
#define NOGDI
#define NOMINMAX
#include <juce_core/juce_core.h>
#include <map>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include <time.h>
#ifdef ENABLE_REGEX_URL
#include <regex>
#endif

#ifndef _MSC_VER
#include <pthread.h>
#include <unistd.h>
#else
#include <windows.h>
typedef HANDLE pthread_mutex_t;
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#endif

#ifndef WIN32
#include <unistd.h>
#endif

#define HTTP_OK 200
#define HTTP_NOT_FOUND 404
#define HTTP_FORBIDDEN 403
#define HTTP_SERVER_ERROR 500

#define WEBSOCKET_FIN 0x80

#include "mongoose/mongoose_api.h"
#include "mongoose/cpp_wrapper/mongoose_Utils.h"
#include "mongoose/cpp_wrapper/mongoose_UploadFile.h"
#include "mongoose/cpp_wrapper/mongoose_Mutex.h"
#include "mongoose/cpp_wrapper/mongoose_Response.h"
#include "mongoose/cpp_wrapper/mongoose_StreamResponse.h"
#include "mongoose/cpp_wrapper/mongoose_JsonResponse.h"
#include "mongoose/cpp_wrapper/mongoose_Request.h"
#include "mongoose/cpp_wrapper/mongoose_RequestHandler.h"
#include "mongoose/cpp_wrapper/mongoose_Session.h"
#include "mongoose/cpp_wrapper/mongoose_Sessions.h"
#include "mongoose/cpp_wrapper/mongoose_WebSocket.h"
#include "mongoose/cpp_wrapper/mongoose_WebSockets.h"
#include "mongoose/cpp_wrapper/mongoose_Controller.h"
#include "mongoose/cpp_wrapper/mongoose_WebController.h"
#include "mongoose/cpp_wrapper/mongoose_JsonController.h"
#include "mongoose/cpp_wrapper/mongoose_Server.h"


