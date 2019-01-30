/*
  ==============================================================================

    FTPropStatus.h
    Created: 30 Jan 2019 5:47:03pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class FTPropStatus :
	public ControllableContainer
{
public:
	FTPropStatus(int propID);
	~FTPropStatus();

	int propID;
	BoolParameter * isConnected;
	FloatParameter * voltage;
	Trigger * flash;

	InspectableEditor * getEditor(bool isRoot) override;
};