/*
  ==============================================================================

    BentoWebServer.h
    Created: 4 Feb 2019 1:02:02pm
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
using namespace Mongoose;

class BentoWebServer :
	public ControllableContainer,
	public WebController
{
public:
	juce_DeclareSingleton(BentoWebServer, true);

	BentoWebServer();	
	~BentoWebServer();

	
	void setupRoutes();
	void getBakedShow(Request &request, StreamResponse &response);


private:
	Mongoose::Server server;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BentoWebServer)
};