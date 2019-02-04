#ifdef JUCE_MONGOOSE_H_INCLUDED
/* When you add this cpp file to your project, you mustn't include it in a file where you've
 already included any other headers - just put it inside a file on its own, possibly with your config
 flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
 header files that the compiler may be using.
 */
#error "Incorrect use of JUCE cpp file"
#endif

#include "juce_mongoose.h"

#include "mongoose/mongoose_api.c"
#include "mongoose/cpp_wrapper/mongoose_Utils.cpp"
#include "mongoose/cpp_wrapper/mongoose_UploadFile.cpp"
#include "mongoose/cpp_wrapper/mongoose_Mutex.cpp"
#include "mongoose/cpp_wrapper/mongoose_Response.cpp"
#include "mongoose/cpp_wrapper/mongoose_StreamResponse.cpp"
#include "mongoose/cpp_wrapper/mongoose_JsonResponse.cpp"
#include "mongoose/cpp_wrapper/mongoose_Request.cpp"
#include "mongoose/cpp_wrapper/mongoose_Session.cpp"
#include "mongoose/cpp_wrapper/mongoose_Sessions.cpp"
#include "mongoose/cpp_wrapper/mongoose_WebSocket.cpp"
#include "mongoose/cpp_wrapper/mongoose_WebSockets.cpp"
#include "mongoose/cpp_wrapper/mongoose_Controller.cpp"
#include "mongoose/cpp_wrapper/mongoose_WebController.cpp"
#include "mongoose/cpp_wrapper/mongoose_JsonController.cpp"
#include "mongoose/cpp_wrapper/mongoose_Server.cpp"
