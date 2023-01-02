/*
  ==============================================================================

	PropFlasher.cpp
	Created: 1 Jan 2023 7:36:12pm
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"
#include "BentoSettings.h"

juce_ImplementSingleton(PropFlasher)

PropFlasher::PropFlasher() :
	ControllableContainer("Prop Flasher"),
	Thread("PropFlasher")
{
	fwType = addEnumParameter("Firmware Type", "Type of prop to upload");
	for (auto& def : PropManager::getInstance()->factory.defs)
	{
		FactorySimpleParametricDefinition<Prop>* pDef = (FactorySimpleParametricDefinition<Prop>*)def;
		String fw = pDef->params.getProperty("firmware", "");
		if (fw.isEmpty()) continue;
		fwType->addOption(pDef->type, fw);
	}

	fwType->addOption("Custom", "");




	fwFileParam = addFileParameter("Firmware File", "The firmware.bin file to flash");
	fwFileParam->fileTypeFilter = "*.bin";
	fwFileParam->setEnabled(false);

	flashTrigger = addTrigger("Upload firmware", "Flash all connected props");
	progression = addFloatParameter("Progression", "Progression", 0, 0, 1);
	//setWifiAfterFlash = addBoolParameter("Set Wifi After flash", "Set wifi credentials in flashed props", true);
	setWifiTrigger = addTrigger("Set Wifi to props", "Set wifi (ssid and pass are set in File > Preferences > Bento Settings)");

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

PropFlasher::~PropFlasher()
{
	stopThread(1000);
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
}

void PropFlasher::onContainerTriggerTriggered(Trigger* t)
{
	if (t == flashTrigger) flash();
	else if (t == setWifiTrigger) setAllWifi();
}

void PropFlasher::flash()
{
	stopThread(1000);

#if JUCE_WINDOWS || JUCE_MAC
	if (!firmwareFile.exists())
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

	File flasher = espFolder.getChildFile("esptool");
	File app0Bin = espFolder.getChildFile("boot_app0.bin");
	File bootloaderBin = espFolder.getChildFile("bootloader_qio_80m.bin");
#endif

	if (!flasher.existsAsFile())
	{
		LOGERROR("Flasher file not found. It should be a file \"esptool\" inside Bento's Application folder. Expecting : " << flasher.getFullPathName());
		return;
	}

	startThread();
#else
	LOGWARNING("Flashing not supported for this platform");
#endif

}

void PropFlasher::setAllWifi()
{
	String wifiStr = "wifi.setCredentials " + BentoSettings::getInstance()->wifiSSID->stringValue() + "," + BentoSettings::getInstance()->wifiPass->stringValue() + "\nroot.restart\n";

	LOG("Setting Wifi infos to prop...");
	for (auto& f : flashedDevices)
	{
		SerialDevice* s = SerialManager::getInstance()->getPort(f, true, 115200);
		if (s == nullptr)
		{
			LOGWARNING("Could not connect !");
			continue;
		}
		s->writeString(wifiStr);
		s->close();
		LOG("Wifi is set to " << f->uniqueDescription);
	}

	LOG("All Props wifi are set !");
}


void PropFlasher::run()
{
	Array<SerialDeviceInfo*> infos;

	for (auto& info : SerialManager::getInstance()->portInfos)
	{
		for (auto& vp : PropManager::getInstance()->vidpids)
		{
			if (info->vid == vp.vid && info->pid == vp.pid)
			{
				//SerialDevice* d = SerialManager::getInstance()->getPort(info, true, 115200);
				//if (d == nullptr)
				//{
				//	LOGWARNING("Could not open port " << info->uniqueDescription);
				//	continue;
				//}

				infos.addIfNotAlreadyThere(info);
			}
		}
	}

	LOG("Flashing " << infos.size() << " devices...");


	numFlashingProps = infos.size();
	progression->setValue(0);
	flashedDevices.clear();

	for (auto& i : infos)
	{
		if (!flashProp(i->port))
		{
			LOGERROR("Error flashing " << i->uniqueDescription);
		}
		else
		{
			LOG("Prop " << i->uniqueDescription << " flashed");
			flashedDevices.add(i);
		}
	}

	if (flashedDevices.size() == infos.size())
	{
		LOG("All props flashed !");
	}
	else
	{
		LOGWARNING("Flashed successfully " << flashedDevices.size() << " props out of " << infos.size());
	}

}

bool PropFlasher::flashProp(const String& port)
{
	float startProgression = progression->floatValue();

	String parameters = " --chip esp32 --port " + port + " --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect";
	parameters += " 0xe000 \"" + app0Bin.getFullPathName() + "\"";
	parameters += " 0x1000 \"" + bootloaderBin.getFullPathName() + "\"";
	parameters += " 0x10000 \"" + firmwareFile.getFullPathName() + "\"";
	parameters += " 0x8000 \"" + partitionsFile.getFullPathName() + "\"";

	LOG("Flashing firmware...");
	//LOG("Launch with parameters " + parameters);

	ChildProcess cp;
	cp.start(flasher.getFullPathName() + parameters);

	bool errored = false;

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
			if (prog.size() > 1)
			{
				float relProg = prog[1].getFloatValue() / 100.0f;
				float tProg = startProgression + relProg / numFlashingProps;
				if (tProg != 1)
				{
					progression->setValue(tProg); //not using 1 to avoid double 100% log from partitions and firmware.
					LOG("Flashing... " << (int)(relProg * 100) << "%");
				}
			}
			buffer = lines[lines.size() - 1];
			wait(10);
		}
	}

	progression->setValue(startProgression + 1.0f / numFlashingProps);

	//LOG(cp.readAllProcessOutput());

	if (errored || cp.readAllProcessOutput().toLowerCase().contains("error"))
	{
		return false;
	}

	return true;
}
