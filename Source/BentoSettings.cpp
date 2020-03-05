/*
  ==============================================================================

	BentoSettings.cpp
	Created: 22 Feb 2020 11:02:04pm
	Author:  bkupe

  ==============================================================================
*/

#include "BentoSettings.h"
#include "Prop/PropManager.h"
#include "Prop/props/bento/BentoProp.h"

juce_ImplementSingleton(BentoSettings)

BentoSettings::BentoSettings() :
	ControllableContainer("Bento Settings"),
	flashCC("Flashing"),
	wifiCC("WiFi")
{
	saveAndLoadRecursiveData = true;

	wifiSSID = wifiCC.addStringParameter("WiFi SSID", "The ssid to save in the prop for connecting", "");
	wifiPass = wifiCC.addStringParameter("WiFi Password", "The password to save in the prop for connecting", "");
	saveWifiTrigger = wifiCC.addTrigger("Save credentials", "Save the credentials into all connected props");
	addChildControllableContainer(&wifiCC);

	arduinoPath = flashCC.addFileParameter("ESP32 Path", "Path to the ESP32 folder that includes \"tools\" and \"hardware\" subfolders");
	arduinoPath->directoryMode = true;
	firmwareFile = flashCC.addFileParameter("Firmware", "Firmware to flash to compatible props");
	flashConnected = flashCC.addTrigger("Flash firmware", "Flash the firmware to connected props");
	addChildControllableContainer(&flashCC);

}

BentoSettings::~BentoSettings()
{
}

void BentoSettings::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{

	if (c == flashConnected)
	{
		File fwFile = firmwareFile->getFile();
		File esp32F = arduinoPath->getFile();
		if (!fwFile.existsAsFile() || !esp32F.exists())
		{
			NLOGERROR(niceName, "Firmware file or flasher not found");
			return;

		}

		File partitionsFwFile = fwFile.getChildFile("../" + fwFile.getFileNameWithoutExtension() + ".partitions.bin");
		if (!partitionsFwFile.exists())
		{
			NLOGERROR(niceName, "Partitions file not found : " << partitionsFwFile.getFullPathName());
			return;
		}
		
		String fileName = "esptool";
#if JUCE_WINDOWS
		fileName += ".exe";
#endif
		File flasher = esp32F.getChildFile("packages/esp32/tools/esptool_py/2.6.1/"+fileName);
		File espFolder = esp32F.getChildFile("packages/esp32/hardware/esp32");
		Array<File> toolsFolders = espFolder.findChildFiles(File::TypesOfFileToFind::findDirectories, true, "tools");

		LOG("Found " << toolsFolders.size() << "tools folder");
		for (auto& tf : toolsFolders) LOG(" > " + tf.getFullPathName());

		if (toolsFolders.size() == 0) return;
		File toolsFolder = toolsFolders[0];


		File app0Bin = toolsFolder.getChildFile("partitions/boot_app0.bin");
		File bootloaderBin = toolsFolder.getChildFile("sdk/bin/bootloader_qio_80m.bin");

		for (auto& p : PropManager::getInstance()->items)
		{
			if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
			{
				if (bp->serialDevice == nullptr) continue;
				String port = bp->serialDevice->info->port;

				bp->serialParam->setValueForDevice(nullptr); //close device to let the flasher use it

				String parameters = " --chip esp32 --port " + port + " --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect";
				parameters += " 0xe000 " + app0Bin.getFullPathName();
				parameters += " 0x1000 " + bootloaderBin.getFullPathName();
				parameters += " 0x10000 " + fwFile.getFullPathName();
				parameters += " 0x8000 " + partitionsFwFile.getFullPathName();

				LOG("Launch with parameters " + parameters);
				flasher.startAsProcess(parameters);

			}
		}
	}
	else if (c == saveWifiTrigger)
	{
		for (auto& p : PropManager::getInstance()->items)
		{
			if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
			{
				if (bp->serialDevice == nullptr) continue;

				bp->sendWiFiCredentials(wifiSSID->stringValue(), wifiPass->stringValue());
			}
		}
	}

}
