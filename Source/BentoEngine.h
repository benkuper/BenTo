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


	StringParameter* projectName;
	GenericControllableManager customParams;

	void clearInternal() override;

	juce::Result saveDocument(const File& file) override;

	void processMessage(const OSCMessage &m, const String& clientId) override;
	var getJSONData(bool includeNonOverriden = false) override;
	void loadJSONDataInternalEngine(var data, ProgressTask * loadingTask) override;

};