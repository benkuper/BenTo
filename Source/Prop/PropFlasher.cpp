/*
  ==============================================================================

	PropFlasher.cpp
	Created: 1 Jan 2023 7:36:12pm
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"
#include "BentoSettings.h"
#include "PropFlasher.h"

juce_ImplementSingleton(PropFlasher)

PropFlasher::PropFlasher() :
	ControllableContainer("Firmware Uploader"),
	Thread("Prop Flasher Wifi"),
	setWifiNoDelay(false),
	propFlasherNotifier(5)
{

	filterKnownDevices = addBoolParameter("Filter Known Devices", "Only upload firmware on devices that are compatible. If you don't see your connect props on the list, try disabling this option.", true);

	updateFirmwareDefinitionsTrigger = addTrigger("Update List", "Update the list of available firmwares");

	fwType = addEnumParameter("Firmware Type", "Type of prop to upload");
	fwVersion = addEnumParameter("Firmware Version", "Version of the firmware to upload");

	fwFileParam = addFileParameter("Firmware File", "The folder of the firmware to flash");
	fwFileParam->directoryMode = true;
	fwFileParam->setEnabled(false);
	forceSlowFlash = addBoolParameter("Force Slow Flash", "Force using the slow flashing method (slower but more reliable)", false);
	fullFlash = addBoolParameter("Full Flash", "Erase the full flash of the prop, not only the program area (needed for some major version changes). This will loose the settings !", false);

	flashTrigger = addTrigger("Upload firmware", "Flash all connected props");
	progression = addFloatParameter("Progression", "Progression", 0, 0, 1);
	progression->setControllableFeedbackOnly(true);
	setWifiAfterFlash = addBoolParameter("Set Wifi During flash", "Set wifi credentials in flashed props", true);
	wifiSSID = addStringParameter("Name", "Name (SSID) for the wifi to set", "");
	wifiPass = addStringParameter("Pass", "Password for the wifi to set", "");
	setAllWifiTrigger = addTrigger("Only Set Wifi", "Set wifi to all connected props but don't flash the firmware");

	serverFilesParam = addFileParameter("Server Files", "Files to upload to the server");
	serverFilesParam->directoryMode = true;
	File sf = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/server");
	if (sf.exists()) serverFilesParam->setValue(sf.getFullPathName());

	uploadTrigger = addTrigger("Upload", "Upload files to the server");
	otaUploadTrigger = addTrigger("OTA Upload", "Upload firmware using OTA");

	updateFirmwareDefinitions();

}

PropFlasher::~PropFlasher()
{
	stopThread(1000);
}

void PropFlasher::updateFirmwareDefinitions(bool force)
{

	File f = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmwares/firmwareList.json");
	if (force && f.exists()) f.deleteFile();

	if (!f.exists())
	{
		LOG("Downloading firmware list...");
		URL url("https://www.goldengeek.org/blip/download/firmware/getFirmwares.php?list");
		std::unique_ptr<InputStream> stream(url.createInputStream(URL::InputStreamOptions(URL::ParameterHandling::inAddress).withConnectionTimeoutMs(500)));

		if (stream != nullptr)
		{
			File firmwareFolder = f.getParentDirectory();
			if (!firmwareFolder.exists()) firmwareFolder.createDirectory();
			FileOutputStream fos(f);
			fos.writeFromInputStream(*stream, -1);
			fos.flush();
			LOG("Firmware definitions downloaded");
		}
		else
		{
			LOGERROR("Failed to download firmware definitions");
			return;
		}
	}

	availableFirmwares = JSON::fromString(f.loadFileAsString());

	if (!availableFirmwares.isObject())
	{
		LOGERROR("Firmware definitions file is corrupted");
		return;
	}

	fwType->clearOptions();
	fwVersion->clearOptions();

	NamedValueSet deviceProps = availableFirmwares.getDynamicObject()->getProperties();
	for (auto& nv : deviceProps)
	{
		var fw = nv.value;
		String fwName = fw["name"].toString();
		String fwPath = nv.name.toString();
		fwType->addOption(fwName, fwPath);
	}

	fwType->addOption("Custom", "custom");


	String ft = fwType->getValueData().toString();
	fwFileParam->setEnabled(ft == "custom");

	updateVersionEnumForFWType();
	updateCompatibleVIDPIDs();

	propFlasherNotifier.addMessage(new PropFlasherEvent(PropFlasherEvent::DEFINITIONS_UPDATED, this));
}




void PropFlasher::updateVersionEnumForFWType()
{
	if (!availableFirmwares.isObject())
	{
		return;
	}

	fwVersion->clearOptions();


	String fwPath = fwType->getValueData().toString();
	if (fwPath.isEmpty()) return;

	var fwData = availableFirmwares.getProperty(fwPath, var());
	if (fwData.isVoid()) return;

	var versionData = fwData["versions"];

	for (int i = 0; i < versionData.size(); i++)
	{
		var v = versionData[i].toString();
		String label = v + String(i == 0 ? " (latest)" : "");
		fwVersion->addOption(label, v);
	}
}

void PropFlasher::updateCompatibleVIDPIDs()
{
	compatibleVIDs.clear();
	compatiblePIDs.clear();

	if (!availableFirmwares.isObject())
	{
		return;
	}

	String typeName = fwType->getValueKey();
	if (typeName == "custom")
	{
	}
	else
	{
		String fwPath = fwType->getValueData().toString();
		if (fwPath.isEmpty()) return;
		var fwData = availableFirmwares.getProperty(fwPath, var());
		if (fwData.isVoid()) return;

		var vids = fwData["vids"];
		var pids = fwData["pids"];

		for (int j = 0; j < pids.size(); j++)
		{
			int pid = pids[j].toString().startsWith("0x") ? pids[j].toString().getHexValue32() : pids[j].toString().getIntValue();
			compatiblePIDs.addIfNotAlreadyThere(pid);
		}

		for (int j = 0; j < vids.size(); j++)
		{
			int vid = vids[j].toString().startsWith("0x") ? vids[j].toString().getHexValue32() : vids[j].toString().getIntValue();
			compatibleVIDs.addIfNotAlreadyThere(vid);
		}
	}

	propFlasherNotifier.addMessage(new PropFlasherEvent(PropFlasherEvent::DEFINITIONS_UPDATED, this));
}

void PropFlasher::setFlashProgression(SingleFlasher* flasher, float val)
{
	int index = flashers.indexOf(flasher);
	progressions.set(index, val);

	float globalProgression = 0;
	for (auto& p : progressions) globalProgression += p;
	globalProgression /= progressions.size();

	progression->setValue(globalProgression);
}

void PropFlasher::setFlashingDone(SingleFlasher* flasher, FlashResult val)
{
	int index = flashers.indexOf(flasher);
	flasherDones.set(index, val);

	int numSuccess = 0;
	int numProcessed = 0;
	for (auto& d : flasherDones)
	{
		if (d == FlashResult::Success) numSuccess++;
		if (d != FlashResult::None) numProcessed++;
	}

	if (numProcessed < flashers.size()) return;


	if (numSuccess == flashers.size())
	{
		LOG("All props flashed !");
	}
	else
	{
		LOGWARNING(numSuccess << " out of " << flashers.size() << " flashed");
	}


	if (setWifiAfterFlash->boolValue()) startThread(); //set wifi

}
void PropFlasher::onContainerParameterChanged(Parameter* p)
{
	if (p == fwType)
	{
		String typeName = fwType->getValueData().toString();
		fwFileParam->setEnabled(typeName == "custom");

		updateVersionEnumForFWType();
		updateCompatibleVIDPIDs();


		// Enable/disable custom folder selection
	}
	else if (p == filterKnownDevices)
	{
		updateCompatibleVIDPIDs();
	}
	else if (p == fwFileParam)
	{
	}
	else if (p == setWifiAfterFlash)
	{
		wifiSSID->setEnabled(setWifiAfterFlash->boolValue());
		wifiPass->setEnabled(setWifiAfterFlash->boolValue());
	}
}

void PropFlasher::onContainerTriggerTriggered(Trigger* t)
{
	if (t == flashTrigger) flashAll();
	else if (t == uploadTrigger) uploadServerFiles();
	else if (t == otaUploadTrigger) otaUploadFirmware();
	if (t == updateFirmwareDefinitionsTrigger) updateFirmwareDefinitions(true);
	else if (t == setAllWifiTrigger) flashAll(true);
}

Array<SerialDeviceInfo*> PropFlasher::getDevicesToFlash()
{
	Array<SerialDeviceInfo*> infos;


	if (filterKnownDevices->boolValue())
	{
		for (auto& info : SerialManager::getInstance()->portInfos)
		{
			bool foundVID = compatibleVIDs.contains(info->vid);
			bool foundPID = compatiblePIDs.contains(info->pid);

			if (foundVID && foundPID) infos.addIfNotAlreadyThere(info);
		}
	}
	else
	{
		infos.addArray(SerialManager::getInstance()->portInfos);
	}

	return infos;
}

void PropFlasher::flashAll(bool onlySetWifi)
{
#if JUCE_WINDOWS || JUCE_MAC

	flashers.clear();
	progressions.clear();
	flasherDones.clear();

	setWifiNoDelay = onlySetWifi;

	Array<SerialDeviceInfo*> infos = getDevicesToFlash();
	for (auto& info : infos)
	{
		SingleFlasher* flasher = new SingleFlasher(info);
		flashers.add(flasher);
		progressions.add(0);
		flasherDones.add(None);
	}

	if (onlySetWifi)
	{
		setAllWifi();
		return;
	}

	File fwFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmwares/").getChildFile(fwType->getValueData().toString()).getChildFile(fwVersion->getValueData().toString());

	if (!fwFolder.exists())
	{
		fwDownloader.download([this]() {
			flashAll(false);
			}, fwType->getValueData().toString(), fwVersion->getValueData().toString());

		return;
	}

	setupFirmwareFile();

	File appFolder = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory();

#if JUCE_WINDOWS
	flasher = appFolder.getChildFile("esptool.exe");
#elif JUCE_MAC
	File bundle = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory().getParentDirectory();
	File espFolder = bundle.getChildFile("Resources").getChildFile("esptool");

	flasher = espFolder.getChildFile("esptool");
#endif

	if (!flasher.existsAsFile())
	{
		LOGERROR("Flasher file not found. It should be a file \"esptool\" inside Bento's Application folder. Expecting : " << flasher.getFullPathName());
		return;
	}

	LOG("Start flashing " << infos.size() << " props...");
	for (auto& f : flashers) f->startThread();

#endif
}


void PropFlasher::run()
{
	if (!setWifiNoDelay) sleep(500);
	setAllWifi();
}

void PropFlasher::setupFirmwareFile()
{
	File fwFolder;

	if (fwType->getValueKey() == "Custom")
	{
		fwFolder = fwFileParam->getFile();
		if (!fwFolder.exists())
		{
			LOGERROR("No folder provided");
			return;
		}
	}
	else
	{
		File firmwaresFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmwares/");
		fwFolder = firmwaresFolder.getChildFile(fwType->getValueData().toString()).getChildFile(fwVersion->getValueData().toString());
	}

	File manifestFile = fwFolder.getChildFile("manifest.json");

	if (!manifestFile.existsAsFile())
	{
		LOGERROR("No manifest file found in folder " << fwFolder.getFullPathName());
		return;
	}

	firmwareData = JSON::parse(manifestFile.loadFileAsString());

	String fwName = fullFlash->boolValue() ? "firmware_full.bin" : "firmware.bin";
	firmwareFile = fwFolder.getChildFile(fwName);

	if (!firmwareFile.exists())
	{
		LOGERROR("Firmware file doesn't exist !");
		return;
	}
}


void PropFlasher::setAllWifi()
{
	if (wifiSSID->stringValue().isEmpty() || wifiPass->stringValue().isEmpty())
	{
		LOGWARNING("Wifi SSID and Pass must be set to upload");
		return;
	}

	String wifiStr = "wifi.ssid " + wifiSSID->stringValue() + "\nwifi.pass " + wifiPass->stringValue() + "\nsettings.save\n";

	LOG("Setting Wifi infos to prop...");
	if (!setWifiNoDelay) wait(4000);



	Array<SerialDevice*> devices;
	for (auto& f : flashers)
	{
		SerialDevice* s = SerialManager::getInstance()->getPort(f->info, true);

		if (s == nullptr)
		{
			LOGWARNING("Could not connect !");
			continue;
		}
		s->setDTR(false);
		s->setRTS(false);
		s->open();
		s->addSerialDeviceListener(this);
		devices.add(s);
	}

	wait(1000);

	for (auto& s : devices)
	{

		s->writeString(wifiStr);
		s->port->flush();
		//s->close(); //no need to close
	}


	wait(100);
	for (auto& s : devices)
	{

		s->writeString("root.restart\n");
		s->port->flush();
		s->close(); //no need to close
	}

	wait(200);

	for (auto& s : devices) s->removeSerialDeviceListener(this);

	LOG("All Props wifi are set !");
}

void PropFlasher::uploadServerFiles(Prop* specificProp)
{
	Array<BentoProp*> props = PropManager::getInstance()->getItemsWithType<BentoProp>();
	File folder = serverFilesParam->getFile();
	if (!folder.exists()) return;

	Array<File> files = folder.findChildFiles(File::TypesOfFileToFind::findFiles, false);

	if (specificProp != nullptr)
	{
		for (auto& f : files) specificProp->addFileToUpload({ f ,"server" });
	}
	else
	{
		for (auto& p : props)
		{
			for (auto& f : files) p->addFileToUpload({ f,"server" });
		}
	}
}

void PropFlasher::otaUploadFirmware()
{

	File fwFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmwares/").getChildFile(fwType->getValueData().toString()).getChildFile(fwVersion->getValueData().toString());

	if (!fwFolder.exists())
	{
		fwDownloader.download([this]() {
			otaUploadFirmware();
			}, fwType->getValueData().toString(), fwVersion->getValueData().toString());

		return;
	}

	setupFirmwareFile();
	String type = fwType->getValueKey();

	if (!firmwareFile.exists())
	{
		LOGWARNING("Firmware file doesn't exist !");
		return;
	}

	Array<BentoProp*> props = PropManager::getInstance()->getItemsWithType<BentoProp>();
	for (auto& p : props)
	{
		if (p->getDeviceType() != type) continue;
		p->addFileToUpload({ firmwareFile });
	}
}

SingleFlasher::SingleFlasher(SerialDeviceInfo* info) :
	Thread("SingleFlasher (" + info->port + ")"),
	progression(0),
	info(info)
{
}


SingleFlasher::~SingleFlasher()
{
	stopThread(1000);
}


void SingleFlasher::run()
{
	bool result = flashProp();
	if (result)
	{
		LOG("[" + info->port + "] Prop flashed");
	}
	else
	{
		LOGERROR("[" + info->port + "] Error flashing");
	}

	PropFlasher::getInstance()->setFlashingDone(this, result ? PropFlasher::Success : PropFlasher::Fail);
}

void PropFlasher::serialDataReceived(SerialDevice* s, const var& data)
{
	//LOG("Flasher Data Received :\n" << data.toString());
	if (data.toString().contains(wifiSSID->stringValue()) && data.toString().contains(wifiPass->stringValue()))
	{
		LOG("Wifi is set for " << s->info->uniqueDescription);
	}
}


bool SingleFlasher::flashProp()
{

	String quotes = "\"";
#if JUCE_MAC
	quotes = "";
#endif

	var data = PropFlasher::getInstance()->firmwareData;
	String chipType = data["chip"];
	String parameters = " --chip " + chipType;
	parameters += " --port " + info->port;

	NamedValueSet& espOptions = data["espOptions"].getDynamicObject()->getProperties();
	for (auto& nv : espOptions)
	{
		String valStr = nv.value.toString();
		if (nv.name.toString() == "baud" && PropFlasher::getInstance()->forceSlowFlash->boolValue()) valStr = "115200";

		parameters += " --" + nv.name + " " + valStr + " ";
	}

	parameters += " write_flash -z";


	NamedValueSet& flashOptions = data["flashOptions"].getDynamicObject()->getProperties();
	for (auto& nv : flashOptions)
	{
		parameters += " --" + nv.name + " " + nv.value.toString() + " ";
	}

	String fwOffset = PropFlasher::getInstance()->fullFlash->boolValue() ? "0x00000" : data["firmwareOffset"].toString();

	parameters += " " + fwOffset + " " + quotes + PropFlasher::getInstance()->firmwareFile.getFullPathName() + quotes;


	LOG("[" + info->port + "] Flashing firmware with parameters :");
	LOG(parameters);

	ChildProcess cp;
	cp.start(PropFlasher::getInstance()->flasher.getFullPathName() + parameters);

	bool errored = false;

	bool got100 = false;
	String buffer;
	while (!errored && cp.isRunning())
	{
		char buf[8];
		memset(buf, 0, 8);
		int numRead = cp.readProcessOutput(buf, 8);
		buffer += String(buf, numRead);
		StringArray lines;
		lines.addLines(buffer);
		for (int i = 0; i < lines.size() - 1; i++)
		{
			if (lines[i].toLowerCase().contains("error"))
			{
				errored = true;
				break;
			}

			StringArray prog = RegexFunctions::getFirstMatch("Writing.+\\(([0-9]+) %\\)", lines[i]);

			//LOG(lines[i]);
			if (prog.size() > 1)
			{
				float relProg = prog[1].getFloatValue() / 100.0f;
				if (relProg != 1)
				{
					PropFlasher::getInstance()->setFlashProgression(this, relProg); //not using 1 to avoid double 100% log from partitions and firmware.

					//LOG("[" + info->port + "] Flashing... " << (int)(relProg * 100) << " % ");
				}
				else
				{
					got100 = true;
				}
			}
			buffer = lines[lines.size() - 1];
			wait(10);
		}
	}

	LOG(buffer);

	if (!got100)
	{
		LOGERROR("[" + info->port + "] Something got wrong");
		LOGERROR("[" + info->port + "] " + cp.readAllProcessOutput());
		errored = true;
	}

	PropFlasher::getInstance()->setFlashProgression(this, 1);

	if (errored || cp.readAllProcessOutput().toLowerCase().contains("error"))
	{
		return false;
	}

	return true;
}

void PropFlasher::FirmwareDownloader::download(std::function<void()> callback, String device, String version)
{
	deviceToDownload = device;
	versionToDownload = version;
	onDownloaded = callback;
	startThread();
}


void PropFlasher::FirmwareDownloader::run()
{
	URL url("https://www.goldengeek.org/blip/download/firmware/getFirmwares.php?device=" + deviceToDownload + "&version=" + versionToDownload);

	LOG("Downloading firmware files for " << deviceToDownload << " : " << versionToDownload << " from " << url.toString(true));

	File target = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmware.zip");
	downloadTask = url.downloadToFile(target, URL::DownloadTaskOptions().withListener(this));
}

void PropFlasher::FirmwareDownloader::progress(URL::DownloadTask* task, int64 bytesDownloaded, int64 totalLength)
{
	if (Time::getMillisecondCounter() - lastLogTime > 1000)
	{
		if (totalLength > 0)
		{
			float prog = (float)bytesDownloaded / (float)totalLength;
			LOG("Downloading firmware files... " << (int)(prog * 100) << " % ");
		}
		else
		{
			LOG("Downloading firmware files... " << bytesDownloaded << " bytes downloaded");
		}
		lastLogTime = Time::getMillisecondCounter();
	}
}

void PropFlasher::FirmwareDownloader::finished(URL::DownloadTask* task, bool success)
{
	if (!success)
	{
		LOGERROR("Error download firmware files");
		return;
	}

	LOG("Firmware files downloaded : " << deviceToDownload << ", version " << versionToDownload);
	ZipFile zip(task->getTargetLocation());
	zip.uncompressTo(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmwares"), true);
	task->getTargetLocation().deleteFile();

	onDownloaded();
}
