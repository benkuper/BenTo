/*
  ==============================================================================

    LightBlockLayerPanel.cpp
    Created: 17 Apr 2018 7:15:39pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockLayerPanel.h"

LightBlockLayerPanel::LightBlockLayerPanel(LightBlockLayer * layer) :
	SequenceLayerPanel(layer),
	blockLayer(layer)
{
	defaultLayerUI = blockLayer->defaultLayer->createToggle();
	targetIdUI = blockLayer->targetId->createStepper();
	defaultLayerUI->showLabel = true;
	targetIdUI->showLabel = true;

	addAndMakeVisible(defaultLayerUI);
	addAndMakeVisible(targetIdUI);
}

LightBlockLayerPanel::~LightBlockLayerPanel()
  {
  }

void LightBlockLayerPanel::resizedInternalContent(Rectangle<int>& r)
{
	defaultLayerUI->setBounds(r.removeFromTop(20));
	targetIdUI->setBounds(r.removeFromTop(20));
}
