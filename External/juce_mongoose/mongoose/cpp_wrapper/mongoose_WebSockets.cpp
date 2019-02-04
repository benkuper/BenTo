
namespace Mongoose
{
    WebSockets::WebSockets(bool responsible_)
        : responsible(responsible_), id(0)
    {
    }

    WebSockets::~WebSockets()
    {
        if (responsible) {
            std::vector<WebSocket *> toDelete;

            std::map<struct mg_connection *, WebSocket *>::iterator it;

            for (it=websockets.begin(); it!=websockets.end(); it++) {
                toDelete.push_back((*it).second);
            }

            std::vector<WebSocket *>::iterator vit;

            for (vit=toDelete.begin(); vit!=toDelete.end(); vit++) {
                remove(*vit);
            }
        }
    }

    void WebSockets::add(WebSocket *websocket)
    {
        if (websocket == NULL) {
            return;
        }

        if (responsible) {
            mutex.lock();
            int newId = id++;
            mutex.unlock();
            websocket->setId(newId);
        }

        struct mg_connection *connection = websocket->getConnection();

        mutex.lock();
        if (websockets.find(connection) != websockets.end()) {
            remove(websockets[connection], false);
        }

        websocketsById[websocket->getId()] = websocket;
        websockets[connection] = websocket;
        mutex.unlock();
    }

    WebSocket *WebSockets::getWebSocket(int identifier)
    {
        if (websocketsById.find(identifier) != websocketsById.end()) {
            return websocketsById[identifier];
        }

        return NULL;
    }

    void WebSockets::sendAll(std::string data)
    {
        std::vector<WebSocket *> toClean;
        std::map<struct mg_connection *, WebSocket *>::iterator it;

        mutex.lock();
        for (it=websockets.begin(); it!=websockets.end(); it++) {
            WebSocket *websocket = (*it).second;

            websocket->send(data);
        }
        mutex.unlock();

        clean();
    }

    void WebSockets::remove(WebSocket *websocket, bool lock)
    {
        struct mg_connection *connection = websocket->getConnection();

        if (lock) {
            mutex.lock();
        }
        if (websockets.find(connection) != websockets.end()) {
            websocket->removeContainer(this);
            websockets.erase(connection);
            websocketsById.erase(websocket->getId());

            if (responsible) {
                websocket->close();
                websocket->notifyContainers();
                delete websocket;
            }
        }
        if (lock) {
            mutex.unlock();
        }
    }

    WebSocket *WebSockets::getWebSocket(struct mg_connection *connection)
    {
        if (websockets.find(connection) != websockets.end()) {
            return websockets[connection];
        }

        return NULL;
    }

    void WebSockets::clean()
    {
        std::vector<WebSocket *> toDelete;
        std::map<struct mg_connection *, WebSocket *>::iterator it;

        mutex.lock();
        for (it=websockets.begin(); it!=websockets.end(); it++) {
            if ((*it).second->isClosed()) {
                toDelete.push_back((*it).second);
            }
        }

        std::vector<WebSocket *>::iterator vit;
        for (vit=toDelete.begin(); vit!=toDelete.end(); vit++) {
            remove(*vit, false);
        }
        mutex.unlock();
    }
};
