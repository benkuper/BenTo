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

Array<Colour> TimelineBlockSequence::getColors(int id, int resolution, double time, var params)
{
	Array<LightBlockLayer *> layers = getLayersForID(id);

	int numLayers = layers.size();

	float t = params.getProperty("sequenceTime", true) ? currentTime->floatValue() : time;
	if(numLayers == 1) return layers[0]->getColors(id, resolution, t, params); //use sequence's time instead of prop time

	Array<Colour> result;
	result.resize(resolution);
	result.fill(Colours::black);

	if (numLayers == 0) return result;

	Array<Array<Colour>> colors;
	for (auto &l : layers)
	{
		String s = l->niceName;
		colors.add(l->getColors(id, resolution, t, params)); //use sequence's time instead of prop time
	}

	for (int i = 0; i < resolution; i++)
	{
		float r = 0, g = 0, b = 0;
		for (int j = 0; j < numLayers; j++)
		{
			r += colors[j][i].getFloatRed();
			g += colors[j][i].getFloatGreen();
			b += colors[j][i].getFloatBlue();
		}

		result.set(i, Colour::fromFloatRGBA(jmin(r, 1.f), jmin(g, 1.f), jmin(b, 1.f),1));
	}

	return result;

}

Array<LightBlockLayer *> TimelineBlockSequence::getLayersForID(int id)
{
	if (layerManager == nullptr) return nullptr; 
	

	Array<LightBlockLayer *> defaultLayers;
	Array<LightBlockLayer *> result;
	
	for (auto &i : layerManager->items)
	{
		LightBlockLayer * l = dynamic_cast<LightBlockLayer *>(i);
		if (l == nullptr) continue;
		if (l->targetId->intValue() == id || l->globalLayer->boolValue()) result.add(l);
		if (l->defaultLayer->boolValue()) defaultLayers.add(l);
	}

	return result.size() > 0 ? result : defaultLayers;
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

}
