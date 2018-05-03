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
	globalLayerUI = blockLayer->globalLayer->createToggle();

	targetIdUI = blockLayer->targetId->createStepper();
	defaultLayerUI->showLabel = true;
	targetIdUI->showLabel = true;

	addAndMakeVisible(defaultLayerUI);
	addAndMakeVisible(targetIdUI);
	addAndMakeVisible(globalLayerUI);
	minContentHeight = 80;
}

LightBlockLayerPanel::~LightBlockLayerPanel()
  {
  }

void LightBlockLayerPanel::resizedInternalContent(Rectangle<int>& r)
{
	Rectangle<int> br = r.removeFromTop(16);
	defaultLayerUI->setBounds(br.removeFromRight(60));
	globalLayerUI->setBounds(br.removeFromLeft(60));
	r.removeFromTop(2);

	targetIdUI->setBounds(r.removeFromTop(20));
}
