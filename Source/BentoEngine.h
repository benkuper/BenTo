/*
  ==============================================================================

    BentoEngine.h
    Created: 10 Apr 2018 5:14:40pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class BentoEngine : 
	public Engine,
	public OSCRemoteControl::RemoteControlListener
{
public:
	BentoEngine();
	~BentoEngine();


	ControllableContainer ioCC;
	StringParameter * remoteHost;
	IntParameter * remotePort;
	OSCSender globalSender;
	StringParameter* projectName;

	void clearInternal() override;

	juce::Result saveDocument(const File& file) override;

	void processMessage(const OSCMessage &m) override;
	var getJSONData() override;
	void loadJSONDataInternalEngine(var data, ProgressTask * loadingTask) override;

};