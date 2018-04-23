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

Array<Colour> TimelineBlockSequence::getColors(int id, int resolution, float /*time*/, var params)
{
	LightBlockLayer * l = getLayerForID(id);

	if (l == nullptr)
	{
		Array<Colour> result;
		result.resize(resolution);
		result.fill(Colours::black);
		return result;
	}

	return l->getColors(id, resolution, currentTime->floatValue(), params); //use sequence's time instead of prop time
}

LightBlockLayer * TimelineBlockSequence::getLayerForID(int id)
{
	LightBlockLayer * defaultLayer = nullptr;

	for (auto &i : layerManager->items)
	{
		LightBlockLayer * l = static_cast<LightBlockLayer *>(i);
		if (l->targetId->intValue() == id) return l;
		if (l->defaultLayer->boolValue()) defaultLayer = l;
	}

	return defaultLayer;
}

void TimelineBlockSequence::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	Sequence::onControllableFeedbackUpdateInternal(cc, c);

	LightBlockLayer * l = dynamic_cast<LightBlockLayer *>(c->parentContainer);
	if (l != nullptr)
	{
		if (c == l->defaultLayer && l->defaultLayer->boolValue())
		{
			for (auto &i : layerManager->items)
			{
				if (i == l) continue; 
				LightBlockLayer * il = dynamic_cast<LightBlockLayer *>(i);
				if(il != nullptr) il->defaultLayer->setValue(false);
			}
		}
	}
}
