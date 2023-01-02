/*
  ==============================================================================

    BentoSettings.h
    Created: 22 Feb 2020 11:02:04pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class BentoSettings : public ControllableContainer
{
public:
	juce_DeclareSingleton(BentoSettings, true);

	BentoSettings();
	~BentoSettings();
	
	ControllableContainer wifiCC;
	StringParameter * wifiSSID;
	StringParameter * wifiPass;
	Trigger* saveWifiTrigger;

	void onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;
};