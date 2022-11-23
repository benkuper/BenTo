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
		for (auto& p : PropManager::getInstance()->items) p->uploadFirmwareTrigger->trigger();
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
