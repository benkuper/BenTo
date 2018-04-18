/*
  ==============================================================================

	LightBlockLayerTimeline.cpp
	Created: 17 Apr 2018 7:15:44pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockLayerTimeline.h"

LightBlockLayerTimeline::LightBlockLayerTimeline(LightBlockLayer * layer) :
	SequenceLayerTimeline(layer),
	managerUI(this, &layer->blockClipManager)
{
	addAndMakeVisible(managerUI);
}

LightBlockLayerTimeline::~LightBlockLayerTimeline()
{
}

void LightBlockLayerTimeline::resized()
{
	managerUI.setBounds(getLocalBounds());
}

void LightBlockLayerTimeline::updateContent()
{
	managerUI.updateContent();
}
