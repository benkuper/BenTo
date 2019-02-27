/*
  ==============================================================================

    LightBlockClipManager.cpp
    Created: 17 Apr 2018 5:11:44pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipManager.h"

LightBlockClipManager::LightBlockClipManager(LightBlockLayer * layer) :
	LayerBlockManager(),
	layer(layer)
{
	itemDataType = "LightBlockClip";
}


LightBlockClipManager::~LightBlockClipManager()
{
}

LayerBlock * LightBlockClipManager::createItem()
{
	return new LightBlockClip(layer);
}