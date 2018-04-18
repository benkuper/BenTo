/*
  ==============================================================================

	TimelineBlockSequence.cpp
	Created: 17 Apr 2018 8:30:13pm
	Author:  Ben

  ==============================================================================
*/

#include "TimelineBlockSequence.h"
#include "layers/Block/LightBlockLayer.h"

TimelineBlockSequence::TimelineBlockSequence() :
	Sequence()
{
	layerManager->factory.layerDefs.add(SequenceLayerDefinition::createDef("Blocks", &LightBlockLayer::create));
	layerManager->factory.buildPopupMenu();
}

TimelineBlockSequence::~TimelineBlockSequence()
{
}

void TimelineBlockSequence::updateColorsForBlock(LightBlock * b, var params)
{
	LightBlockLayer * l = getLayerForPropID(b->prop->id->intValue());
	if (l == nullptr) return;
	l->updateColorsForBlock(b, params);
}

LightBlockLayer * TimelineBlockSequence::getLayerForPropID(int id)
{
	for (auto &i : layerManager->items)
	{
		LightBlockLayer * l = static_cast<LightBlockLayer *>(i);
		if (l != nullptr) return l;
	}

	return nullptr;
}
