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
	public LayerBlockManager,
	public LightBlockClip::ClipListener
{
public:
	LightBlockClipManager(LightBlockLayer * layer);
	~LightBlockClipManager();

	LightBlockLayer * lightBlockLayer;
	LayerBlock * createItem() override;

	void addItemInternal(LayerBlock* clip, var) override;
	void removeItemInternal(LayerBlock* clip) override;

	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable *c) override;

	void clipFadesChanged(LightBlockClip* clip) override;
	void computeFadesForClip(LightBlockClip * clip, bool propagate);
};