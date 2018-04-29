/*
  ==============================================================================

    LightBlockClipManager.cpp
    Created: 17 Apr 2018 5:11:44pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipManager.h"

LightBlockClipManager::LightBlockClipManager() :
	BaseManager("Blocks")
{
	itemDataType = "LightBlockClip";
}

LightBlockClipManager::~LightBlockClipManager()
{
}

LightBlockClip * LightBlockClipManager::addClipAt(float time)
{
	LightBlockClip * t = new LightBlockClip(time);
	BaseManager::addItem(t);
	return t;
}

Array<LightBlockClip*> LightBlockClipManager::getClipsAtTime(float time)
{
	Array<LightBlockClip*> result;
	for (auto &c : items) if (c->isInRange(time)) result.add(c);
	return result;
}
