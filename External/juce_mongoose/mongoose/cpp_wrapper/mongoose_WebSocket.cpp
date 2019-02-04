
namespace Mongoose
{
    WebSocket::WebSocket(struct mg_connection *connection_)
        : id(-1), data(""), request(connection_), connection(connection_), closed(false)
    {
    }

    void WebSocket::setId(int id_)
    {
        id = id_;
    }

    int WebSocket::getId()
    {
        return id;
    }

    void WebSocket::appendData(std::string data_)
    {
        data += data_;
    }

    std::string WebSocket::flushData()
    {
        std::string oldData = "";
        data.swap(oldData);

        return oldData;
    }

    Request &WebSocket::getRequest()
    {
        return request;
    }

    void WebSocket::send(std::string dataToSend, int opcode)
    {
        if (isClosed()) {
            return;
        }

        mutex.lock();
        if (!mg_websocket_write(connection, opcode, dataToSend.c_str(), dataToSend.size())) {
            closed = true;
        }
        mutex.unlock();
    }

    void WebSocket::notifyContainers()
    {
        std::vector<WebSockets *>::iterator it;

        mutex.lock();
        for (it=containers.begin(); it!=containers.end(); it++) {
            (*it)->remove(this);
        }
        mutex.unlock();
    }

    void WebSocket::close()
    {
        closed = true;
    }

    bool WebSocket::isClosed()
    {
        return closed;
    }

    void WebSocket::addContainer(WebSockets *websockets)
    {
        mutex.lock();
        containers.push_back(websockets);
        mutex.unlock();
    }

    void WebSocket::removeContainer(WebSockets *websockets)
    {
        mutex.lock();
        std::vector<WebSockets *>::iterator it;

        for (it=containers.begin(); it!=containers.end(); it++) {
            if (*it == websockets) {
                containers.erase(it);
                break;
            }
        }
        mutex.unlock();
    }

    struct mg_connection *WebSocket::getConnection()
    {
        return connection;
    }
};

