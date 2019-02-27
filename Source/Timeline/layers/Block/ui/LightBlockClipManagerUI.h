/*
  ==============================================================================

    LightBlockClipManagerUI.h
    Created: 17 Apr 2018 7:20:49pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockClipManager.h"
#include "LightBlockClipUI.h"

class LightBlockLayerTimeline;

class LightBlockClipManagerUI :
	public LayerBlockManagerUI
{
public:
	LightBlockClipManagerUI(LightBlockLayerTimeline * timeline, LightBlockClipManager * manager);
	~LightBlockClipManagerUI();

	LightBlockClipManager * clipManager;
	LightBlockLayerTimeline * timeline;

	LayerBlockUI * createUIForItem(LayerBlock * item) override;
};
