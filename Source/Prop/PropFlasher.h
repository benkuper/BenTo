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
	public Thread,
	public SerialDevice::SerialDeviceListener
{
public:
	juce_DeclareSingleton(PropFlasher, true);
	PropFlasher();
	~PropFlasher();

	EnumParameter* fwType;
	File firmwareFile;
	File partitionsFile;

	FileParameter* fwFileParam;
	FloatParameter* progression;
	BoolParameter* setWifiAfterFlash;
	StringParameter* wifiSSID;
	StringParameter* wifiPass;

	Trigger* flashTrigger;
	//Trigger* setWifiTrigger;

	File flasher;
	File app0Bin;
	File bootloaderBin;

	Array<SerialDeviceInfo*> flashedDevices;

	int numFlashingProps;

	void onContainerParameterChanged(Parameter* p) override;
	void onContainerTriggerTriggered(Trigger* t) override;

	void flash();
	void setAllWifi();

	void run() override;

	void serialDataReceived(SerialDevice* s, const var& data) override;

	bool flashProp(const String& port);

};