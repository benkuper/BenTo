/*
  ==============================================================================

	TimelineBlockSequence.cpp
	Created: 17 Apr 2018 8:30:13pm
	Author:  Ben

  ==============================================================================
*/

#include "TimelineBlockSequence.h"
#include "layers/Block/LightBlockLayer.h"
#include "Audio/AudioManager.h"

TimelineBlockSequence::TimelineBlockSequence() :
	Sequence()
{
	layerManager->factory.layerDefs.add(SequenceLayerDefinition::createDef("Blocks", &LightBlockLayer::create));
	layerManager->factory.layerDefs.add(SequenceLayerDefinition::createDef("Audio", &AudioLayer::create));
	layerManager->factory.buildPopupMenu();

	layerManager->addBaseManagerListener(this);

	setAudioDeviceManager(&AudioManager::getInstance()->am);
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
	if (layerManager == nullptr) return nullptr; 
	
	LightBlockLayer * defaultLayer = nullptr;
	
	for (auto &i : layerManager->items)
	{
		LightBlockLayer * l = dynamic_cast<LightBlockLayer *>(i);
		if (l == nullptr) continue;
		if (l->targetId->intValue() == id) return l;
		if (l->defaultLayer->boolValue()) defaultLayer = l;
	}

	return defaultLayer;
}

void TimelineBlockSequence::itemAdded(SequenceLayer * s)
{

	LightBlockLayer * l = dynamic_cast<LightBlockLayer *>(s);
	if (l != nullptr)
	{
		if (!Engine::mainEngine->isLoadingFile && layerManager->items.size() == 1) l->defaultLayer->setValue(true);
		return;
	}

	AudioLayer * al = dynamic_cast<AudioLayer *>(s);
	if (al != nullptr)
	{
		al->setAudioProcessorGraph(&AudioManager::getInstance()->graph, AUDIO_OUTPUT_GRAPH_ID);
	}

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
