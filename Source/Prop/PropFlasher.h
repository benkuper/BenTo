/*
  ==============================================================================

	PropFlasher.h
	Created: 1 Jan 2023 7:36:12pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class SingleFlasher :
	public Thread,
	public SerialDevice::SerialDeviceListener
{
public:
	SingleFlasher(const String& port);
	~SingleFlasher();

	String port;
	float progression;

	void run() override;
	bool flashProp();
};

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

	enum FlashResult { None, Success, Fail };
	OwnedArray<SingleFlasher> flashers;
	Array<float> progressions;
	Array<FlashResult> flasherDones;

	void setFlashProgression(SingleFlasher*, float val);;
	void setFlashingDone(SingleFlasher*, FlashResult val);

	void onContainerParameterChanged(Parameter* p) override;
	void onContainerTriggerTriggered(Trigger* t) override;

	Array<SerialDeviceInfo*> getDevicesToFlash();

	void flashAll();
	void setAllWifi();
	void uploadServerFiles();

	void run();

	void serialDataReceived(SerialDevice* s, const var& data) override;

};