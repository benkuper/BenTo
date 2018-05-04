/*
  ==============================================================================

	LightBlock.cpp
	Created: 10 Apr 2018 6:56:47pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock.h"

#include "LightBlock.h"
#include "Prop/Prop.h"

LightBlock::LightBlock(LightBlockColorProvider * provider) :
	BaseItem(provider->niceName),
	provider(provider),
	paramsContainer("Parameters"),
	paramsLoadData(var()),
    automationsManager(&paramsContainer)
{
	addChildControllableContainer(&paramsContainer);

	userCanRemove = false;

	rebuildArgsFromModel();
	provider->addColorProviderListener(this);

	addChildControllableContainer(&automationsManager);
}

LightBlock::~LightBlock()
{
	if (!provider.wasObjectDeleted()) provider->removeColorProviderListener(this);
}

Array<Colour> LightBlock::getColors(int id, int resolution, float time, var params)
{
	var localParams = params.isVoid()?new DynamicObject():new DynamicObject(*params.getDynamicObject());
	Array<WeakReference<Parameter>> paramList = paramsContainer.getAllParameters();
	

	if (localParams.getProperty("updateAutomation", true))
	{
		for (auto &a : automationsManager.items) a->automation.position->setValue(fmodf(time, a->automation.positionMax));
	} else
	{
		for (auto &a : automationsManager.items)
		{
			if (a->parameter != nullptr && !a->parameter.wasObjectDeleted())
			{
				float value = a->automation.getValueForPosition(fmodf(time, a->automation.positionMax));
				float normValue = jmap<float>(value, a->parameter->minimumValue, a->parameter->maximumValue);
				localParams.getDynamicObject()->setProperty(a->parameter->shortName,normValue);
			}
		}
	}

	for (auto &p : paramList)
	{
		if (p.wasObjectDeleted() || p == nullptr) continue;
		if(!localParams.hasProperty(p->shortName)) localParams.getDynamicObject()->setProperty(p->shortName, p->value);
	}

	if (provider.wasObjectDeleted())
	{
		Array<Colour> result;
		result.resize(resolution);
		result.fill(Colours::black);
		return result;
	}

	return provider->getColors(id, resolution, time, localParams);
}

void LightBlock::rebuildArgsFromModel()
{
	if(paramsContainer.controllables.size() > 0) paramsLoadData = paramsContainer.getJSONData();
	var aData = automationsManager.getJSONData();

	paramsContainer.clear();
	automationsManager.clear();

	Array<WeakReference<Controllable>> params = provider->getModelParameters();

	for (auto &sc : params)
	{
		if (sc->type == Controllable::TRIGGER)
		{
			 paramsContainer.addTrigger(sc->niceName, sc->description);
		} else
		{
			Parameter * p = ControllableFactory::createParameterFrom(sc, true, true);
			p->setControllableFeedbackOnly(sc->isControllableFeedbackOnly);
			paramsContainer.addParameter(p);
		}
	}

	paramsContainer.hideInEditor = paramsContainer.controllables.size() == 0;
	paramsContainer.loadJSONData(paramsLoadData);

	automationsManager.hideInEditor = paramsContainer.hideInEditor;
	automationsManager.loadJSONData(aData);

}


void LightBlock::providerParametersChanged(LightBlockColorProvider *)
{
	rebuildArgsFromModel(); 
}

void LightBlock::providerParameterValueUpdated(LightBlockColorProvider *, Parameter * p)
{
	
	if (p == nullptr) return;
	Parameter * tp = paramsContainer.getParameterByName(p->shortName);
	if (tp == nullptr) return;

	if (tp->isOverriden) return;
	tp->defaultValue = p->value;
	tp->resetValue();
}

var LightBlock::getJSONData()
{
	var data = BaseItem::getJSONData();
	data.getDynamicObject()->setProperty("params", paramsContainer.getJSONData());
	data.getDynamicObject()->setProperty("automations", automationsManager.getJSONData());
	
	return data;

}

void LightBlock::loadJSONDataInternal(var data)
{
	
	BaseItem::loadJSONDataInternal(data);

	var pData = data.getProperty("params", var());
	paramsContainer.loadJSONData(pData);
	automationsManager.loadJSONData(data.getProperty("automations", var()));

	if (paramsContainer.controllables.size() == 0) paramsLoadData = pData; //if params where not already there when loading (using script for exemple), store data to use later
}
