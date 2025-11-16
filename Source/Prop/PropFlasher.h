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
		String deviceToDownload;
		String versionToDownload;

		unsigned long lastLogTime = 0;


		void download(std::function<void()> callback, String device, String version);
		void run() override;
		void progress(URL::DownloadTask* task, int64 bytesDownloaded, int64 totalLength) override;
		void finished(URL::DownloadTask* task, bool success) override;

	};
	FirmwareDownloader fwDownloader;

	BoolParameter* filterKnownDevices;

	EnumParameter* fwType;
	EnumParameter* fwVersion;
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
	BoolParameter* forceSlowFlash;
	BoolParameter* fullFlash;

	FileParameter* serverFilesParam;
	Trigger* uploadTrigger;
	Trigger* otaUploadTrigger;

	File flasher;

	int numFlashingProps;
	var availableFirmwares;
	Array<int> compatibleVIDs;
	Array<int> compatiblePIDs;

	enum FlashResult { None, Success, Fail };
	OwnedArray<SingleFlasher> flashers;
	Array<float> progressions;
	Array<FlashResult> flasherDones;

	void updateFirmwareDefinitions(bool force = false);

	void updateVersionEnumForFWType();
	void updateCompatibleVIDPIDs();

	void setFlashProgression(SingleFlasher*, float val);;
	void setFlashingDone(SingleFlasher*, FlashResult val);

	void onContainerParameterChanged(Parameter* p) override;
	void onContainerTriggerTriggered(Trigger* t) override;

	Array<SerialDeviceInfo*> getDevicesToFlash();

	void flashAll(bool onlySetWifi = false);
	void setAllWifi();
	void uploadServerFiles(Prop* specificProp = nullptr);
	void otaUploadFirmware();

	void run();

	void setupFirmwareFile();

	void serialDataReceived(SerialDevice* s, const var& data) override;

	DECLARE_ASYNC_EVENT(PropFlasher, PropFlasher, propFlasher, ENUM_LIST(DEFINITIONS_UPDATED), EVENT_NO_CHECK);

	// Inherited via DownloadTaskListener
};