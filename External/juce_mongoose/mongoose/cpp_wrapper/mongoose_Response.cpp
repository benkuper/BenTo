
namespace Mongoose
{
    Response::Response() : code(HTTP_OK), headers()
    {
    }
            
    Response::~Response()
    {
    }
            
    void Response::setHeader(std::string key, std::string value)
    {
        headers[key] = value;
    }

    bool Response::hasHeader(std::string key)
    {
        return headers.find(key) != headers.end();
    }

    std::string Response::getData()
    {
        std::string body = getBody();
        std::ostringstream data;

        data << "HTTP/1.0 " << code << "\r\n";

        if (!hasHeader("Content-Length")) {
            std::ostringstream length;
            length << body.size();
            setHeader("Content-Length", length.str());
        }

        std::map<std::string, std::string>::iterator it;
        for (it=headers.begin(); it!=headers.end(); it++) {
            data << (*it).first << ": " << (*it).second << "\r\n";
        }

        data << "\r\n";

        data << body;

        return data.str();
    }

    void Response::setCookie(std::string key, std::string value)
    {
        std::ostringstream definition;
        definition << key << "=" << value << "; path=/";

        setHeader("Set-cookie", definition.str());
    }

    void Response::setCode(int code_)
    {
        code = code_;
    }
}
