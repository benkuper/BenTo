/*
  ==============================================================================

    LightBlockLayerTimeline.h
    Created: 17 Apr 2018 7:15:44pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockLayer.h"
#include "LightBlockClipManagerUI.h"

class LightBlockLayerTimeline :
	public SequenceLayerTimeline
{
public:
	LightBlockLayerTimeline(LightBlockLayer * layer);
	~LightBlockLayerTimeline();

	LightBlockLayer * blockLayer;
	LightBlockClipManagerUI managerUI;

	void resized() override;
	void updateContent() override;

	virtual void setSeekManipulationMode(bool isManipulating) override;

};