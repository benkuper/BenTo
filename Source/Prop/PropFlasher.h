/*
  ==============================================================================

	PropFlasher.h
	Created: 1 Jan 2023 7:36:12pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class PropFlasher :
	public ControllableContainer,
	public Thread
{
public:
	juce_DeclareSingleton(PropFlasher, true);
	PropFlasher();
	~PropFlasher();

	File firmwareFile;
	File partitionsFile;

	FileParameter* fwFileParam;
	FloatParameter* progression;
	BoolParameter* setWifiAfterFlash;
	Trigger* flashTrigger;

	File flasher;
	File app0Bin;
	File bootloaderBin;

	int numFlashingProps;

	void onContainerTriggerTriggered(Trigger* t) override;

	void flash();

	void run() override;

	void flashProp(const String& port);

};