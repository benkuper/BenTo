/*
  ==============================================================================

    LightBlockLayerPanel.h
    Created: 17 Apr 2018 7:15:39pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockLayer.h"

class LightBlockLayerPanel :
	public SequenceLayerPanel
{
public:
	LightBlockLayerPanel(LightBlockLayer * layer);
	~LightBlockLayerPanel();

	ScopedPointer<BoolToggleUI> defaultLayerUI;
	ScopedPointer<IntStepperUI> targetIdUI;
	
	LightBlockLayer * blockLayer;

	void resizedInternalContent(Rectangle<int> &r) override;
};
