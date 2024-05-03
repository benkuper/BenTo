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
	setWifiNoDelay(false)
{

	filterKnownDevices = addBoolParameter("Filter Known Devices", "Only upload firmware on devices that are compatible. If you don't see your connect props on the list, try disabling this option.", true);

	updateFirmwareDefinitionsTrigger = addTrigger("Update Firmware Definitions", "Update the list of available firmwares");

	fwType = addEnumParameter("Firmware Type", "Type of prop to upload");

	fwFileParam = addFileParameter("Firmware File", "The firmware.bin file to flash");
	fwFileParam->fileTypeFilter = "*.bin";
	fwFileParam->setEnabled(false);

	flashTrigger = addTrigger("Upload firmware", "Flash all connected props");
	progression = addFloatParameter("Progression", "Progression", 0, 0, 1);
	setWifiAfterFlash = addBoolParameter("Set Wifi After flash", "Set wifi credentials in flashed props", true);
	wifiSSID = addStringParameter("Wifi SSID", "SSID for the wifi to set", "");
	wifiPass = addStringParameter("Wifi Pass", "Pass for the wifi to set", "");
	setAllWifiTrigger = addTrigger("Set Wifi Only", "Set wifi to all connected props");

	

	//testing here, should come from internet

	var fwData(new DynamicObject());
	fwData.getDynamicObject()->setProperty("name", "Flowtoys Creators Club");
	var vids;
	vids.append(0x10c4);
	var pids;
	pids.append(0xea60);
	fwData.getDynamicObject()->setProperty("vids", vids);
	fwData.getDynamicObject()->setProperty("pids", pids);
	fwData.getDynamicObject()->setProperty("version", "1.0.0");
	fwData.getDynamicObject()->setProperty("date", "2021-01-01");
	fwData.getDynamicObject()->setProperty("firmware", "");
	fwData.getDynamicObject()->setProperty("filename", "creatorclub");

	availableFirmwares.append(fwData);

	serverFilesParam = addFileParameter("Server Files", "Files to upload to the server");
	serverFilesParam->directoryMode = true;
	File sf = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/server");
	if (sf.exists()) serverFilesParam->setValue(sf.getFullPathName());

	uploadTrigger = addTrigger("Upload", "Upload files to the server");

	updateFirmwareDefinitions();

}

PropFlasher::~PropFlasher()
{
	stopThread(1000);
}

void PropFlasher::updateFirmwareDefinitions()
{
	fwType->clearOptions();
	File f = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmwares");
	if (!f.exists())
	{
		LOG("No firmware folder found, downloading...");
		fwDownloader.download([this]() { updateFirmwareDefinitions(); });
		return;
	}

	Array<File> fwFolders = f.findChildFiles(File::TypesOfFileToFind::findDirectories, false);

	for (auto& fold : fwFolders)
	{
		if (fold.getFileName() == "server")
		{
			if (serverFilesParam->stringValue().isEmpty())  serverFilesParam->setValue(fold.getFullPathName());
			continue; //ignore server folder for firmware listing
		}

		if (!fold.getChildFile("firmware.bin").existsAsFile() || !fold.getChildFile("partitions.bin").existsAsFile())
		{
			NLOGWARNING(niceName, "Firmware folder " << fold.getFullPathName() << " is missing firmware.bin or partitions.bin");
			continue;
		}
		fwType->addOption(fold.getFileName(), fold.getFullPathName() + "/firmware.bin");
	}

	fwType->addOption("Custom", "");

	String ft = fwType->getValueData().toString();
	fwFileParam->setEnabled(ft.isEmpty());

	if (ft.isEmpty())
	{
		firmwareFile = fwFileParam->getFile();
	}
	else
	{
		firmwareFile = File(fwType->getValueData().toString());
	}
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
		String ft = fwType->getValueData().toString();
		fwFileParam->setEnabled(ft.isEmpty());

		if (ft.isEmpty())
		{
			firmwareFile = fwFileParam->getFile();
		}
		else
		{
			firmwareFile = File(fwType->getValueData().toString());
		}
	}
	else if (p == fwFileParam)
	{
		if (fwFileParam->enabled) firmwareFile = fwFileParam->getFile();
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
	if (t == updateFirmwareDefinitionsTrigger) updateFirmwareDefinitions();
	else if (t == setAllWifiTrigger) flashAll(true);
}

Array<SerialDeviceInfo*> PropFlasher::getDevicesToFlash()
{
	Array<SerialDeviceInfo*> infos;


	if (filterKnownDevices->boolValue())
	{
		for (auto& info : SerialManager::getInstance()->portInfos)
		{
			bool foundVID = false;
			bool foundPID = false;

			for (int i = 0; i < availableFirmwares.size(); i++)
			{
				var vids = availableFirmwares[i].getDynamicObject()->getProperty("vids");
				var pids = availableFirmwares[i].getDynamicObject()->getProperty("pids");

				for (int n = 0; n < vids.size(); n++)
				{
					if ((int)vids[n] == info->vid)
					{
						foundVID = true;
						break;
					}
				}

				for (int n = 0; n < pids.size(); n++)
				{
					if ((int)pids[n] == info->pid)
					{
						foundPID = true;
						break;
					}
				}
			}

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


	if (!firmwareFile.exists())
	{
		LOGERROR("Firmware file doesn't exist !");
		return;
	}

	partitionsFile = firmwareFile.getChildFile("../partitions.bin");

	if (!partitionsFile.exists())
	{
		LOGERROR("Partitions file not found. It should be a file called partitions.bin aside the firmware.bin file");
		return;
	}

	File appFolder = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory();

#if JUCE_WINDOWS
	flasher = appFolder.getChildFile("esptool.exe");
	app0Bin = appFolder.getChildFile("boot_app0.bin");
	bootloaderBin = appFolder.getChildFile("bootloader_qio_80m.bin");
#elif JUCE_MAC
	File bundle = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory().getParentDirectory();
	File espFolder = bundle.getChildFile("Resources").getChildFile("esptool");

	flasher = espFolder.getChildFile("esptool");
	app0Bin = espFolder.getChildFile("boot_app0.bin");
	bootloaderBin = espFolder.getChildFile("bootloader_qio_80m.bin");

#endif

	if (!flasher.existsAsFile())
	{
		LOGERROR("Flasher file not found. It should be a file \"esptool\" inside Bento's Application folder. Expecting : " << flasher.getFullPathName());
		return;
	}

	if (!app0Bin.existsAsFile() || !bootloaderBin.existsAsFile())
	{
		LOGERROR("Bootloader files not found :\n" << app0Bin.getFullPathName() << "\n" << bootloaderBin.getFullPathName());
		return;
	}

	

	LOG("Start flashing " << infos.size() << " props...");
	for (auto& f : flashers) f->startThread();

#endif
}


void PropFlasher::run()
{
	if(!setWifiNoDelay) sleep(500);
	setAllWifi();
}


void PropFlasher::setAllWifi()
{
	if (wifiSSID->stringValue().isEmpty() || wifiPass->stringValue().isEmpty())
	{
		LOGWARNING("Wifi SSID and Pass must be set to upload");
		return;
	}

	String wifiStr = "wifi.ssid " + wifiSSID->stringValue() + "\nwifi.pass " + wifiPass->stringValue() + "\nroot.saveSettings\nroot.restart\n";

	LOG("Setting Wifi infos to prop...");
	if (!setWifiNoDelay) wait(2000);



	Array<SerialDevice*> devices;
	for (auto& f : flashers)
	{
		SerialDevice* s = SerialManager::getInstance()->getPort(f->info, true);
		
		if (s == nullptr)
		{
			LOGWARNING("Could not connect !");
			continue;
		}
		s->setDTR(true);
		s->setRTS(true);
		s->addSerialDeviceListener(this);
		devices.add(s);
	}


	for (auto& s : devices)
	{

		s->writeString(wifiStr);
		s->port->flush();
		//s->close(); //no need to close
	}


	wait(1000);
	for (auto& s : devices)
	{

		s->writeString("root.restart");
		s->port->flush();
		s->close(); //no need to close
	}

	wait(500);

	for (auto& s : devices) s->removeSerialDeviceListener(this);

	LOG("All Props wifi are set !");
}

void PropFlasher::uploadServerFiles()
{
	Array<BentoProp*> props = PropManager::getInstance()->getItemsWithType<BentoProp>();
	File folder = serverFilesParam->getFile();
	if (!folder.exists()) return;

	Array<File> files = folder.findChildFiles(File::TypesOfFileToFind::findFiles, false);

	for (auto& p : props)
	{
		for (auto& f : files) p->addFileToUpload({ f,"server" });
	}
}


SingleFlasher::SingleFlasher(SerialDeviceInfo* info) :
	Thread("SingleFlasher (" + info->port + ")"),
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
	if (data.toString().contains(wifiSSID->stringValue() + " : " + wifiPass->stringValue()))
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

	String parameters = " --chip esp32 --port " + info->port + " --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect";
	parameters += " 0xe000 " + quotes + PropFlasher::getInstance()->app0Bin.getFullPathName() + quotes;
	parameters += " 0x1000 " + quotes + PropFlasher::getInstance()->bootloaderBin.getFullPathName() + quotes;
	parameters += " 0x10000 " + quotes + PropFlasher::getInstance()->firmwareFile.getFullPathName() + quotes;
	parameters += " 0x8000 " + quotes + PropFlasher::getInstance()->partitionsFile.getFullPathName() + quotes;

	LOG("[" + info->port + "] Flashing firmware...");
	//LOG("Launch with parameters " + parameters);

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
					LOG("[" + info->port + "] Flashing... " << (int)(relProg * 100) << " % ");
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

void PropFlasher::FirmwareDownloader::download(std::function<void()> callback)
{
	onDownloaded = callback;
	startThread();
}

void PropFlasher::FirmwareDownloader::run()
{
	URL url("https://benjamin.kuperberg.fr/bentuino/getFirmwares.php");
	File target = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmwares.zip");
	downloadTask = url.downloadToFile(target, URL::DownloadTaskOptions().withListener(this));
}

void PropFlasher::FirmwareDownloader::progress(URL::DownloadTask* task, int64 bytesDownloaded, int64 totalLength)
{
	//LOG("Downloading firmware files...");
}

void PropFlasher::FirmwareDownloader::finished(URL::DownloadTask* task, bool success)
{
	if (!success)
	{
		LOGERROR("Error download firmware files");
		return;
	}

	LOG("Firmware files downloaded");
	ZipFile zip(task->getTargetLocation());
	zip.uncompressTo(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/firmwares"), true);
	task->getTargetLocation().deleteFile();

	onDownloaded();
}
