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
	paramsLoadData(var())
    //automationsManager(&paramsContainer)
{
	addChildControllableContainer(&paramsContainer);

	userCanRemove = false;

	rebuildArgsFromModel();
	provider->addColorProviderListener(this);

	//addChildControllableContainer(&automationsManager);
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
		for (auto &p : paramList)
		{
			if (p->controlMode != Parameter::AUTOMATION) continue;
			p->automation->currentTime->setValue(fmodf(time, p->automation->automation.length->floatValue()));
		}
	} else
	{
		for (auto &p : paramList)
		{
			if (p->controlMode != Parameter::AUTOMATION) continue;
			Automation * a = &p->automation->automation;

			float value = a->getValueForPosition(fmodf(time, a->length->floatValue()));
			float normValue = jmap<float>(value, p->minimumValue, p->maximumValue);
			localParams.getDynamicObject()->setProperty(p->shortName,normValue);
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
	//var aData = automationsManager.getJSONData();

	paramsContainer.clear();
	//automationsManager.clear();

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
			p->addParameterListener(this);
			paramsContainer.addParameter(p);
		}
	}

	paramsContainer.hideInEditor = paramsContainer.controllables.size() == 0;
	paramsContainer.loadJSONData(paramsLoadData);

	//automationsManager.hideInEditor = paramsContainer.hideInEditor;
	//automationsManager.loadJSONData(aData);

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
    
    //set the default value even if overriden, so when doing a manual "reset value", it's back to the actual current one.
    tp->defaultValue = p->value;
    
	if (tp->isOverriden) return;
	if (tp->controlMode != Parameter::MANUAL) return;
	
	tp->resetValue();
}

void LightBlock::parameterControlModeChanged(Parameter * p)
{
	if (p->controlMode == Parameter::AUTOMATION)
	{
		p->automation->mode->setValueWithData(PlayableParameterAutomation::MANUAL);
		//p->automation->hideInEditor = true;
	}

	blockListeners.call(&LightBlockListener::blockParamControlModeChanged, p);
}

var LightBlock::getJSONData()
{
	var data = BaseItem::getJSONData();
	data.getDynamicObject()->setProperty("params", paramsContainer.getJSONData());
	//data.getDynamicObject()->setProperty("automations", automationsManager.getJSONData());
	
	return data;

}

void LightBlock::loadJSONDataInternal(var data)
{
	
	BaseItem::loadJSONDataInternal(data);

	var pData = data.getProperty("params", var());
	paramsContainer.loadJSONData(pData);
	//automationsManager.loadJSONData(data.getProperty("automations", var()));

	if (paramsContainer.controllables.size() == 0) paramsLoadData = pData; //if params where not already there when loading (using script for exemple), store data to use later
}
