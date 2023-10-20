/*
  ==============================================================================

	SequenceBlockSequence.cpp
	Created: 17 Apr 2018 8:30:13pm
	Author:  Ben

  ==============================================================================
*/

#include "Audio/AudioManager.h"
#include "Sequence/SequenceIncludes.h"
#include "SequenceBlockSequence.h"

SequenceBlockSequence::SequenceBlockSequence() :
	Sequence(),
	currentIdentityGroup(nullptr)
{
	identityClusterGroup = addEnumParameter("Calibration Group", "The cluster group to choose to show the calibration frame");
	identityMode = addBoolParameter("Identity mode", "If checked, this will override the colors to show identity", false);

	layerManager->managerFactory = &layerFactory;
	clusterGroupManager.addBaseManagerListener(this);
	addChildControllableContainer(&clusterGroupManager);


	//Timeline
	layerFactory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Blocks", &LightBlockLayer::create, this));
	layerFactory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Audio", &AudioLayer::create, this));
	layerFactory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Video", &VideoLayer::create, this));

	if (!Engine::mainEngine->isLoadingFile)
	{
		layerManager->addItem(new LightBlockLayer(this));
	}

	layerManager->addBaseManagerListener(this);
	setAudioDeviceManager(&AudioManager::getInstance()->am);

	updateGroupList();
}

SequenceBlockSequence::~SequenceBlockSequence()
{
}

Array<Colour> SequenceBlockSequence::getColors(Prop* p, double time, var params)
{
	if (identityMode->boolValue() && currentIdentityGroup != nullptr)
	{
		return currentIdentityGroup->getColorsForProp(p);
	}

	Array<LightBlockLayer*> layers = getLayersForProp(p);

	int numLayers = layers.size();
	float t = params.getProperty("sequenceTime", true) ? currentTime->floatValue() : time;

	if (numLayers == 1 && layers[0] != nullptr)
	{
		params.getDynamicObject()->setProperty("forceID", layers[0]->filterManager->getTargetIDForProp(p));
		return layers[0]->getColors(p, t, params); //use sequence's time instead of prop time
	}

	int resolution = p->getResolution();

	Array<Colour> result;
	result.resize(resolution);
	result.fill(Colours::transparentBlack);

	if (numLayers == 0) return result;

	Array<Array<Colour>> colors;
	Array<LightBlockLayer::BlendMode> blendModes;

	int numActiveLayers = 0;
	for (auto& l : layers)
	{
		if (l == nullptr) continue;

		params.getDynamicObject()->setProperty("forceID", l->filterManager->getTargetIDForProp(p));
		Array<Colour> cols = l->getColors(p, t, params); //use sequence's time instead of prop time

		if (cols.isEmpty()) continue;

		colors.add(cols);
		blendModes.add(l->blendMode->getValueDataAsEnum<LightBlockLayer::BlendMode>());

		numActiveLayers++;
	}

	for (int i = 0; i < resolution; i++)
	{
		float r = 0, g = 0, b = 0, a = 0;
		for (int j = numActiveLayers - 1; j >= 0; j--)
		{
			Colour c = colors.getUnchecked(j).getUnchecked(i);
			switch (blendModes[j])
			{
			case LightBlockLayer::ADD:
				r += c.getFloatRed();
				g += c.getFloatGreen();
				b += c.getFloatBlue();
				a += c.getFloatAlpha();
				break;

			case LightBlockLayer::ALPHA:
			{
				float ca = c.getFloatAlpha();
				r = r + (c.getFloatRed() - r) * ca;
				g = g + (c.getFloatGreen() - g) * ca;
				b = b + (c.getFloatBlue() - b) * ca;
				a += ca;
			}
			break;

			case LightBlockLayer::MASK:
			{
				float br = c.getBrightness();
				float ma = c.getFloatAlpha();
				float fac = jmap<float>(ma, 1, br);
				r *= fac;// jmin(r, colors[j][i].getFloatRed())* a;
				g *= fac;// jmin(g, colors[j][i].getFloatGreen())* a;
				b *= fac;// jmin(b, colors[j][i].getFloatBlue())* a;
				a *= fac;
			}
			break;
			}

		}

		result.set(i, Colour::fromFloatRGBA(jmin(r, 1.f), jmin(g, 1.f), jmin(b, 1.f), jmin(a, 1.f)));
	}

	return result;

}

Array<LightBlockLayer*> SequenceBlockSequence::getLayersForProp(Prop* p, bool includeDisabled)
{

	if (layerManager == nullptr) return nullptr;
	if (Engine::mainEngine->isClearing) return nullptr;

	Array<LightBlockLayer*> defaultLayers;
	Array<LightBlockLayer*> result;

	for (auto& i : layerManager->items)
	{
		if (!includeDisabled && !i->enabled->boolValue()) continue;
		LightBlockLayer* l = dynamic_cast<LightBlockLayer*>(i);
		if (l == nullptr) continue;
		if (l->filterManager->getTargetIDForProp(p) >= 0) result.add(l);
		if (l->defaultLayer->boolValue()) defaultLayers.add(l);
	}

	return result.size() > 0 ? result : defaultLayers;
}

var SequenceBlockSequence::getWasmTimingsDataForProp(Prop* p, float timeOffset)
{
	var result;
	Array<LightBlockLayer*> layers = getLayersForProp(p);
	for (auto& layer : layers)
	{
		Array<LightBlockClip*> clips = layer->getClipsOfTypeForProp<EmbeddedScriptBlock>(p);

		clips.sort(layer->blockClipManager.comparator);

		for (auto& c : clips)
		{
			EmbeddedScriptBlock* b = (EmbeddedScriptBlock*)c->currentBlock->provider.get();
			if (b == nullptr) continue;

			var bData(new DynamicObject());
			bData.getDynamicObject()->setProperty("name", b->shortName);
			bData.getDynamicObject()->setProperty("start", c->time->floatValue() + timeOffset);
			bData.getDynamicObject()->setProperty("end", c->getEndTime() + timeOffset);

			result.append(bData);
		}
	}
	return result;
}

void SequenceBlockSequence::itemAdded(SequenceLayer* item)
{

	LightBlockLayer* l = dynamic_cast<LightBlockLayer*>(item);
	if (l != nullptr)
	{
		if (!Engine::mainEngine->isLoadingFile && layerManager->items.size() == 1) l->defaultLayer->setValue(true);
		return;
	}

	AudioLayer* al = dynamic_cast<AudioLayer*>(item);
	if (al != nullptr)
	{
		al->setAudioProcessorGraph(&AudioManager::getInstance()->graph, AUDIO_OUTPUT_GRAPH_ID);
	}
}

void SequenceBlockSequence::itemsAdded(Array<SequenceLayer*> items)
{
	for (auto& item : items)
	{
		LightBlockLayer* l = dynamic_cast<LightBlockLayer*>(item);
		if (l != nullptr)
		{
			if (!Engine::mainEngine->isLoadingFile && layerManager->items.size() == 1) l->defaultLayer->setValue(true);
			return;
		}

		AudioLayer* al = dynamic_cast<AudioLayer*>(item);
		if (al != nullptr)
		{
			al->setAudioProcessorGraph(&AudioManager::getInstance()->graph, AUDIO_OUTPUT_GRAPH_ID);
		}
	}

}

void SequenceBlockSequence::itemAdded(PropClusterGroup* g)
{
	identityClusterGroup->addOption(g->niceName, g->shortName);
	currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
}

void SequenceBlockSequence::itemsAdded(Array<PropClusterGroup*> groups)
{
	for (auto& g : groups) identityClusterGroup->addOption(g->niceName, g->shortName);
	currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
}

void SequenceBlockSequence::itemRemoved(PropClusterGroup* g)
{
	identityClusterGroup->removeOption(g->niceName);
	currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
}

void SequenceBlockSequence::itemsRemoved(Array<PropClusterGroup*> groups)
{
	for (auto& g : groups) identityClusterGroup->removeOption(g->niceName);
	currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
}

void SequenceBlockSequence::updateGroupList()
{
	String oldValue = identityClusterGroup->getValueData().toString();
	identityClusterGroup->clearOptions();
	identityClusterGroup->addOption("No group", "");
	for (auto& g : clusterGroupManager.items) identityClusterGroup->addOption(g->niceName, g->shortName);

	identityClusterGroup->setValueWithData(oldValue);
	currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
}

var SequenceBlockSequence::getJSONData()
{
	var data = Sequence::getJSONData();
	data.getDynamicObject()->setProperty("clusterGroups", clusterGroupManager.getJSONData());
	return data;
}

void SequenceBlockSequence::loadJSONDataInternal(var data)
{
	Sequence::loadJSONDataInternal(data);
	clusterGroupManager.loadJSONData(data.getProperty("clusterGroups", var()));
}

void SequenceBlockSequence::onContainerParameterChangedInternal(Parameter* p)
{
	Sequence::onContainerParameterChangedInternal(p);

	if (p == identityClusterGroup)
	{
		currentIdentityGroup = clusterGroupManager.getItemWithName(identityClusterGroup->getValueData());
	}
	else if (p == identityMode)
	{
		//
	}
}

void SequenceBlockSequence::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	Sequence::onControllableFeedbackUpdateInternal(cc, c);

}
