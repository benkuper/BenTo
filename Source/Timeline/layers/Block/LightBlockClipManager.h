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
	public BaseManager<LightBlockClip>
{
public:
	LightBlockClipManager(LightBlockLayer * layer);
	~LightBlockClipManager();

	LightBlockLayer * layer;

	LightBlockClip * createItem() override;

	LightBlockClip * addClipAt(float time);
	Array<LightBlockClip *> getClipsAtTime(float time);
};