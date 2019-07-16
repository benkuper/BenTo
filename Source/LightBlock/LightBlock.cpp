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
	BaseItem(provider->niceName, false),
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

Array<Colour> LightBlock::getColors(Prop * p, double time, var params)
{
	var localParams = params.isVoid()?new DynamicObject():new DynamicObject(*params.getDynamicObject());
	Array<WeakReference<Parameter>> paramList = paramsContainer.getAllParameters();
	
	paramsLock.enter();

	if (localParams.getProperty("updateAutomation", true))
	{
		for (auto &param : paramList)
		{
			if (param.wasObjectDeleted()) continue;
			if (param->controlMode != Parameter::AUTOMATION  || param->automation == nullptr) continue;
			param->automation->currentTime->setValue(fmodf(time, param->automation->automation.length->floatValue()));
		}
	} else
	{
		for (auto &param : paramList)
		{
			if (param->controlMode != Parameter::AUTOMATION) continue;
			Automation * a = &param->automation->automation;

			float value = a->getValueForPosition(fmodf(time, a->length->floatValue()));
			float normValue = jmap<float>(value, param->minimumValue, param->maximumValue);
			localParams.getDynamicObject()->setProperty(param->shortName,normValue);
		}
	}

	for (auto &param : paramList)
	{
		if (param.wasObjectDeleted() || param == nullptr) continue;
		if(!localParams.hasProperty(param->shortName)) localParams.getDynamicObject()->setProperty(param->shortName, param->getValue());
	}

	paramsLock.exit();

	if (provider.wasObjectDeleted())
	{
		Array<Colour> result;
		result.resize(p->resolution->intValue());
		result.fill(Colours::black);
		return result;
	}



	return provider->getColors(p, time, localParams);
}

void LightBlock::rebuildArgsFromModel()
{
	if(paramsContainer.controllables.size() > 0) paramsLoadData = paramsContainer.getJSONData();
	//var aData = automationsManager.getJSONData();

	paramsLock.enter();
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

	paramsLock.exit();

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
    tp->defaultValue = p->getValue();
    
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

BakeData LightBlock::getBakeDataForProp(Prop * p)
{
	BakeData result = (!provider.wasObjectDeleted() && provider != nullptr) ? provider->getBakeDataForProp(p) : BakeData(shortName);
	result.metaData.getDynamicObject()->setProperty("blockName", niceName);
	return result;
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