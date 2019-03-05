/*
  ==============================================================================

    LightBlockClipManager.h
    Created: 17 Apr 2018 5:11:44pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlockClip.h"

class LightBlockLayer;

class LightBlockClipManager :
	public LayerBlockManager
{
public:
	LightBlockClipManager(LightBlockLayer * layer);
	~LightBlockClipManager();

	LightBlockLayer * layer;
	LayerBlock * createItem() override;

	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable *c) override;

	void computeFadesForClip(LightBlockClip * clip, bool propagate);
};