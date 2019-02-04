# juce_mongoose
A JUCE Module containing C++ Bindings for the popular mongoose lightweight web server

# Mongoose-C++

https://github.com/Gregwar/mongoose-cpp

Mongoose-C++ is a fork of the popular [mongoose](https://github.com/valenok/mongoose)
lightweight web server which aims to add C++ bindings and a easy-to-use 
API.

# Features

- Object-Oriented high level API, keeping the lightweight mongoose implementation
  as a backend
- Easy-to-use controllers sytem to build an application with modules
- Possibility of enabling JsonCPP to create a json compliant web application
- URL dispatcher using regex matches (C++11)
- Session system to store data about an user using cookies and garbage collect cleaning
- Simple access to GET & POST requests
- Websockets support

# Hello world

Here is an example, this will serve the static files from `www/` directory (which
is the default setting) and the `/hello` page will be answered by a controller which
will display the GET `name` variable, for instance `/hello?name=bob` will display
the string "Hello bob". Default parameter value, if not provided, will be
"... waht's your name ?". This is the `helloworld` program build in the examples:

```c++
class MainComponent   : public Component, public Mongoose::WebController
{
public:
    //==============================================================================
    MainComponent()
    : server (8080)
    {
        setupRoutes();
        server.registerController(this);
        server.start();
        
        setSize (600, 400);
    }
    
    void hello (Mongoose::Request &request, Mongoose::StreamResponse &response)
    {
        response << "Hello " << htmlEntities(request.get("name", "... what's your name ?")) << std::endl;
    }
    
    void setupRoutes()
    {
        addRoute("GET", "/hello", MainComponent, hello);
    }

private:
    
    Mongoose::Server server;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
```

# License

The mongoose binding (`mongoose/` folder) is under MIT license

However, the original mongoose project license is different.

See https://github.com/Gregwar/mongoose-cpp for more info.
