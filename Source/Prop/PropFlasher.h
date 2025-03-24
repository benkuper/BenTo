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
	SingleFlasher(SerialDeviceInfo* info);
	~SingleFlasher();

	SerialDeviceInfo* info;

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

	class FirmwareDownloader :
		public Thread,
		public URL::DownloadTaskListener
	{
	public:
		FirmwareDownloader() : Thread("Firmware Downloader") {}
		~FirmwareDownloader() {}

		std::unique_ptr<URL::DownloadTask> downloadTask;
		std::function<void()> onDownloaded;

		void download(std::function<void()> callback);
		void run() override;
		void progress(URL::DownloadTask* task, int64 bytesDownloaded, int64 totalLength) override;
		void finished(URL::DownloadTask* task, bool success) override;

	};
	FirmwareDownloader fwDownloader;

	BoolParameter* filterKnownDevices;

	EnumParameter* fwType;
	var firmwareData;
	File firmwareFile;

	Trigger* updateFirmwareDefinitionsTrigger;
	FileParameter* fwFileParam;
	FloatParameter* progression;
	BoolParameter* setWifiAfterFlash;
	StringParameter* wifiSSID;
	StringParameter* wifiPass;
	Trigger* setAllWifiTrigger;
	bool setWifiNoDelay;

	Trigger* flashTrigger;

	FileParameter* serverFilesParam;
	Trigger* uploadTrigger;

	File flasher;

	int numFlashingProps;
	var availableFirmwares;
	std::unique_ptr<URL::DownloadTask> firmwareDownloadTask;

	enum FlashResult { None, Success, Fail };
	OwnedArray<SingleFlasher> flashers;
	Array<float> progressions;
	Array<FlashResult> flasherDones;

	void updateFirmwareDefinitions(bool force = false);

	void setFlashProgression(SingleFlasher*, float val);;
	void setFlashingDone(SingleFlasher*, FlashResult val);

	void onContainerParameterChanged(Parameter* p) override;
	void onContainerTriggerTriggered(Trigger* t) override;

	Array<SerialDeviceInfo*> getDevicesToFlash();

	void flashAll(bool onlySetWifi = false);
	void setAllWifi();
	void uploadServerFiles();

	void run();

	void serialDataReceived(SerialDevice* s, const var& data) override;

	DECLARE_ASYNC_EVENT(PropFlasher, PropFlasher, propFlasher, ENUM_LIST(DEFINITIONS_UPDATED), EVENT_NO_CHECK);
};