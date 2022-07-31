/*
  ==============================================================================

	BentoSettings.cpp
	Created: 22 Feb 2020 11:02:04pm
	Author:  bkupe

  ==============================================================================
*/

#include "BentoSettings.h"
#include "Prop/PropIncludes.h"


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
#if JUCE_WINDOWS
		File flasher = File::getSpecialLocation(File::currentApplicationFile).getChildFile("../esptool.exe");


		File fwFile = firmwareFile->getFile();
		if (!fwFile.existsAsFile())
		{
			NLOGERROR(niceName, "Firmware file or flasher not found");
			return;

		}

		File partitionsFwFile = fwFile.getChildFile("../partitions.bin");

		if (!partitionsFwFile.exists())
		{
			NLOGERROR(niceName, "Partitions file not found : " << partitionsFwFile.getFullPathName());
			return;
		}
		
		File app0Bin = fwFile.getChildFile("../boot_app0.bin");
		File bootloaderBin = fwFile.getChildFile("../bootloader_qio_80m.bin");

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
#else
		LOGWARNING("Flashing only supported on Windows for now");
#endif

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
