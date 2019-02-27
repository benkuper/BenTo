#pragma warning(disable:4100 4244 4996)

namespace Mongoose
{
    Controller::Controller() 
        : sessions(NULL), server(NULL), prefix("")
    {
    }

    void Controller::setup()
    {
    }

    void Controller::setServer(Server *server_)
    {
        server = server_;
    }

    void Controller::webSocketReady(WebSocket *websocket)
    {
    }

    void Controller::webSocketData(WebSocket *websocket, std::string data)
    {
    }
    
    Controller::~Controller()
    {
        std::map<std::string, RequestHandlerBase *>::iterator it;

        for (it=routes.begin(); it!=routes.end(); it++) {
            delete (*it).second;
        }

        routes.clear();
    }
            
    bool Controller::handles(std::string method, std::string url)
    { 
        std::string key = method + ":" + url;

        return (routes.find(key) != routes.end());
    }

    Response *Controller::process(Request &request)
    {
        Response *response = NULL;

#ifdef ENABLE_REGEX_URL
        map<string, RequestHandlerBase *>::iterator it; 
        for (it=routes.begin(); it!=routes.end(); it++) {
            if (request.match(it->first)){
              response = it->second->process(request);
              break;
            }   
        }   
#else
        std::string key = request.getMethod() + ":" + request.getUrl();
        if (routes.find(key) != routes.end()) {
            response = routes[key]->process(request);
        }
#endif
        
        return response;
    }
            
    void Controller::preProcess(Request &request, Response &response)
    {
    }
            
    void Controller::postProcess(Request &request, Response &response)
    {
    }

    Response *Controller::handleRequest(Request &request)
    {
        Response *response = process(request);

        if (response != NULL) {
            postProcess(request, *response);
        }

        return response;
    }

    void Controller::setPrefix(std::string prefix_)
    {
        prefix = prefix_;
    }
            
    void Controller::registerRoute(std::string httpMethod, std::string route, RequestHandlerBase *handler)
    {
        std::string key = httpMethod + ":" + prefix + route;
        routes[key] = handler;
        urls.push_back(prefix + route);
    }

    void Controller::dumpRoutes()
    {
        std::map<std::string, RequestHandlerBase *>::iterator it;

        for (it=routes.begin(); it!=routes.end(); it++) {
            std::cout << (*it).first << std::endl;
        }

    }

    Response *Controller::serverInternalError(std::string message)
    {
        StreamResponse *response = new StreamResponse;

        response->setCode(HTTP_SERVER_ERROR);
        *response << "[500] Server internal error: " << message;

        return response;
    }

    std::vector<std::string> Controller::getUrls()
    {
        return urls;
    }

    Session &Controller::getSession(Request &request, Response &response)
    {
        return sessions->get(request, response);
    }

    void Controller::setSessions(Sessions *sessions_)
    {
        sessions = sessions_;
    }
}
