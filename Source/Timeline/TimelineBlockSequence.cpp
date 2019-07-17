/*
  ==============================================================================

	TimelineBlockSequence.cpp
	Created: 17 Apr 2018 8:30:13pm
	Author:  Ben

  ==============================================================================
*/

#include "TimelineBlockSequence.h"
#include "layers/Block/LightBlockLayer.h"
#include "layers/Action/ActionLayer.h"
#include "Audio/AudioManager.h"
#include "Prop/Prop.h"

TimelineBlockSequence::TimelineBlockSequence() :
	Sequence(),
	currentIdentityGroup(nullptr)
{
	identityClusterGroup = addEnumParameter("Calibration Group", "The cluster group to choose to show the calibration frame");
	identityMode = addBoolParameter("Identity mode", "If checked, this will override the colors to show identity", false, false);

	layerManager->managerFactory = &layerFactory;
	clusterGroupManager.addBaseManagerListener(this);
	addChildControllableContainer(&clusterGroupManager);

	
	//Timeline
	layerFactory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Blocks", &LightBlockLayer::create, this));
	layerFactory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Actions", &ActionLayer::create, this));
	layerFactory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Audio", &AudioLayer::create, this));

	if (!Engine::mainEngine->isLoadingFile)
	{
		layerManager->addItem(new LightBlockLayer(this, &clusterGroupManager));
	}

	layerManager->addBaseManagerListener(this);
	setAudioDeviceManager(&AudioManager::getInstance()->am);

	updateGroupList();
}

TimelineBlockSequence::~TimelineBlockSequence()
{
}

Array<Colour> TimelineBlockSequence::getColors(Prop * p, double time, var params)
{
	if (identityMode->boolValue() && currentIdentityGroup != nullptr)
	{
		return currentIdentityGroup->getColorsForProp(p);
	}

	Array<LightBlockLayer *> layers = getLayersForProp(p);

	int numLayers = layers.size();
	float t = params.getProperty("sequenceTime", true) ? currentTime->floatValue() : time;
	
	if (numLayers == 1 && layers[0] != nullptr)
	{
		params.getDynamicObject()->setProperty("forceID", layers[0]->filterManager->getTargetIDForProp(p));
		return layers[0]->getColors(p, t, params); //use sequence's time instead of prop time
	}

	int resolution = p->resolution->intValue();
	
	Array<Colour> result;
	result.resize(resolution);
	result.fill(Colours::black);

	if (numLayers == 0) return result;

	Array<Array<Colour>> colors;
	for (auto &l : layers)
	{
		if (l == nullptr) continue;
		String s = l->niceName;
		params.getDynamicObject()->setProperty("forceID", l->filterManager->getTargetIDForProp(p));
		colors.add(l->getColors(p, t, params)); //use sequence's time instead of prop time
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

Array<LightBlockLayer*> TimelineBlockSequence::getLayersForProp(Prop * p, bool includeDisabled)
{

	if (layerManager == nullptr) return nullptr; 
	if (Engine::mainEngine->isClearing) return nullptr;

	Array<LightBlockLayer *> defaultLayers;
	Array<LightBlockLayer *> result;
	
	for (auto &i : layerManager->items)
	{
		if (!includeDisabled && !i->enabled->boolValue()) continue;
		LightBlockLayer * l = dynamic_cast<LightBlockLayer *>(i);
		if (l == nullptr) continue;
		if (l->filterManager->getTargetIDForProp(p) >= 0) result.add(l);
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

void TimelineBlockSequence::itemAdded(PropClusterGroup* g)
{
	identityClusterGroup->addOption(g->niceName, g->shortName);
	currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
}

void TimelineBlockSequence::itemRemoved(PropClusterGroup* g)
{
	identityClusterGroup->removeOption(g->niceName);	
	currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
}

void TimelineBlockSequence::updateGroupList()
{
	String oldValue = identityClusterGroup->getValueData().toString();
	identityClusterGroup->clearOptions();
	identityClusterGroup->addOption("No group", "");
	for (auto& g : clusterGroupManager.items) identityClusterGroup->addOption(g->niceName, g->shortName);

	identityClusterGroup->setValueWithData(oldValue);
	currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
}

var TimelineBlockSequence::getJSONData()
{
	var data = Sequence::getJSONData();
	data.getDynamicObject()->setProperty("clusterGroups", clusterGroupManager.getJSONData());
	return data;
}

void TimelineBlockSequence::loadJSONDataInternal(var data)
{
	Sequence::loadJSONDataInternal(data);
	clusterGroupManager.loadJSONData(data.getProperty("clusterGroups", var()));
}

void TimelineBlockSequence::onContainerParameterChangedInternal(Parameter* p)
{
	Sequence::onContainerParameterChangedInternal(p);

	if (p == identityClusterGroup)
	{
		identityMode->setEnabled(identityClusterGroup->getValueData() != "");
		currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
	}
	else if (p == identityMode)
	{
		//
	}
}

void TimelineBlockSequence::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	Sequence::onControllableFeedbackUpdateInternal(cc, c);

}
