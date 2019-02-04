/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
using namespace Mongoose;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component, public WebController
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
    
    void hello (Request &request, StreamResponse &response)
    {
        response << "Hello " << htmlEntities(request.get("name", "... what's your name ?")) << "\n";
    }
    
    void setupRoutes()
    {
        addRoute("GET", "/hello", MainComponent, hello);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);
        g.setColour (Colours::white);
        g.drawText("Visit http://localhost:8080/hello to see the demo! Click to launch...",
                   0, 0, getWidth(), getHeight(), Justification::centred);
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        URL url ("http://localhost:8080/hello?name=Jules");
        url.launchInDefaultBrowser();
    }
    
private:
    
    Server server;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

