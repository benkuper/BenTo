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

	BoolParameter* filterKnownDevices;

	EnumParameter* fwType;
	File firmwareFile;
	File partitionsFile;

	FileParameter* fwFileParam;
	FloatParameter* progression;
	BoolParameter* setWifiAfterFlash;
	StringParameter* wifiSSID;
	StringParameter* wifiPass;

	Trigger* flashTrigger;

	FileParameter* serverFilesParam;
	Trigger* uploadTrigger;

	File flasher;
	File app0Bin;
	File bootloaderBin;

	Array<SerialDeviceInfo*> flashedDevices;

	int numFlashingProps;

	var availableFirmwares;

	void onContainerParameterChanged(Parameter* p) override;
	void onContainerTriggerTriggered(Trigger* t) override;

	Array<SerialDeviceInfo*> getDevicesToFlash();

	void flash();
	void setAllWifi();
	void uploadServerFiles();

	void run() override;

	void serialDataReceived(SerialDevice* s, const var& data) override;

	bool flashProp(const String& port);

};