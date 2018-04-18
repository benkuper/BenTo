/*
  ==============================================================================

    LightBlockClipManager.h
    Created: 17 Apr 2018 5:11:44pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlockClip.h"

class LightBlockClipManager :
	public BaseManager<LightBlockClip>
{
public:
	LightBlockClipManager();
	~LightBlockClipManager();


	LightBlockClip * addClipAt(float time);
	Array<LightBlockClip *> getClipsAtTime(float time);
};