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

	ControllableContainer flashCC;
	FileParameter* arduinoPath;
	FileParameter* firmwareFile;
	Trigger* flashConnected;

	void onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;
};