/*
  ==============================================================================

	LightBlock.cpp
	Created: 10 Apr 2018 6:56:47pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "Prop/PropIncludes.h"
#include "ParameterLink/ParameterLink.h"

LightBlock::LightBlock(LightBlockColorProvider* provider) :
	ControllableContainer(provider->niceName),
	provider(provider),
	paramsLoadData(var())
{
	paramsContainer.reset(new ParamLinkContainer("Parameters"));

	addChildControllableContainer(paramsContainer.get());

	rebuildArgsFromModel();
	provider->addColorProviderListener(this);
}

LightBlock::~LightBlock()
{
	if (!provider.wasObjectDeleted()) provider->removeColorProviderListener(this);
}

Array<Colour> LightBlock::getColors(Prop* p, double time, var params)
{
	var localParams = getLocalParams(p, time, params);

	if (provider.wasObjectDeleted())
	{
		Array<Colour> result;
		result.resize(p->getResolution());
		result.fill(Colours::transparentBlack);
		return result;
	}

	return provider->getColors(p, time, localParams);
}

void LightBlock::filterColors(Array<Colour>* result, Prop* p, double time, var params)
{
	if (provider.wasObjectDeleted()) return;

	var localParams = getLocalParams(p, time, params);
	int id = params.getProperty("forceID", p->globalID->intValue());
	((LightBlockModel*)provider.get())->getColorsInternal(result, p, time, id, p->getResolution(), localParams);
}

var LightBlock::getLocalParams(Prop* p, double time, var params)
{
	var localParams = params.isVoid() ? new DynamicObject() : new DynamicObject(*params.getDynamicObject());

	paramsLock.enter();
	Array<WeakReference<Parameter>> paramList = paramsContainer->getAllParameters();

	//if (localParams.getProperty("updateAutomation", true))
	//{
	//	for (auto& param : paramList)
	//	{
	//		if (param.wasObjectDeleted()) continue;
	//		if (param->controlMode != Parameter::AUTOMATION || param->automation == nullptr) continue;
	//		//param->automation->timeParamRef->setValue(fmodf(time, param->automation->lengthParamRef->floatValue()));
	//	}
	//}
	//else
	//{
	for (auto& param : paramList)
	{
		if (param->controlMode != Parameter::AUTOMATION) continue;

		if (ParameterAutomation* a = param->automation.get())
		{

			if (dynamic_cast<Automation*>(a->automationContainer) != nullptr)
			{
				float value = ((Automation*)a->automationContainer)->getValueAtPosition(fmodf(time, a->lengthParamRef->floatValue()));
				localParams.getDynamicObject()->setProperty(param->shortName, value);
			}
			else if (dynamic_cast<GradientColorManager*>(a->automationContainer) != nullptr)
			{
				Colour value = ((GradientColorManager*)a->automationContainer)->getColorForPosition(fmodf(time, a->lengthParamRef->floatValue()));
				var colorParam;
				colorParam.append(value.getFloatRed());
				colorParam.append(value.getFloatGreen());
				colorParam.append(value.getFloatBlue());
				colorParam.append(value.getFloatAlpha());
				localParams.getDynamicObject()->setProperty(param->shortName, colorParam);
			}
		}
	}
	//}

	for (auto& param : paramList)
	{
		if (param.wasObjectDeleted() || param == nullptr) continue;
		if (!localParams.hasProperty(param->shortName)) localParams.getDynamicObject()->setProperty(param->shortName, paramsContainer->getLinkedValue(param, p, p->globalID->intValue(), time));
	}

	paramsLock.exit();

	return localParams;
}


void LightBlock::rebuildArgsFromModel()
{
	if (paramsContainer->controllables.size() > 0) paramsLoadData = paramsContainer->getJSONData();
	//var aData = automationsManager.getJSONData();

	paramsLock.enter();
	paramsContainer->clear();
	//automationsManager.clear();


	Array<WeakReference<Controllable>> params = provider->getModelParameters();

	for (auto& sc : params)
	{
		if (sc->type == Controllable::TRIGGER)
		{
			paramsContainer->addTrigger(sc->niceName, sc->description);
		}
		else
		{
			Parameter* p = ControllableFactory::createParameterFrom(sc, true, true);
			p->setControllableFeedbackOnly(sc->isControllableFeedbackOnly);
			p->addParameterListener(this);
			paramsContainer->addParameter(p);
		}
	}

	paramsContainer->hideInEditor = paramsContainer->controllables.size() == 0;
	paramsContainer->loadJSONData(paramsLoadData);

	paramsLock.exit();

	//automationsManager.hideInEditor = paramsContainer->hideInEditor;
	//automationsManager.loadJSONData(aData);

}


void LightBlock::providerParametersChanged(LightBlockColorProvider*)
{
	rebuildArgsFromModel();
}

void LightBlock::providerParameterValueUpdated(LightBlockColorProvider*, Parameter* p)
{

	if (p == nullptr) return;
	Parameter* tp = paramsContainer->getParameterByName(p->shortName);
	if (tp == nullptr) return;

	//set the default value even if overriden, so when doing a manual "reset value", it's back to the actual current one.
	tp->defaultValue = p->getValue();

	if (tp->isOverriden) return;
	if (tp->controlMode != Parameter::MANUAL) return;

	tp->resetValue();
}

void LightBlock::parameterControlModeChanged(Parameter* p)
{
	if (p->controlMode == Parameter::AUTOMATION)
	{
		p->automation->setManualMode(true);
		//p->automation->hideInEditor = true;
	}

	blockListeners.call(&LightBlockListener::blockParamControlModeChanged, p);
}

void LightBlock::handleEnterExit(bool enter, Array<Prop*> props)
{
	if (provider != nullptr && !provider.wasObjectDeleted()) provider->handleEnterExit(enter, props);
}

PlaybackData LightBlock::getPlaybackDataForProp(Prop* p)
{
	PlaybackData result = (!provider.wasObjectDeleted() && provider != nullptr) ? provider->getPlaybackDataForProp(p) : PlaybackData(shortName);
	result.metaData.getDynamicObject()->setProperty("blockName", niceName);
	return result;
}


var LightBlock::getJSONData(bool includeNonOverriden)
{
	var data = ControllableContainer::getJSONData(includeNonOverriden);
	data.getDynamicObject()->setProperty("params", paramsContainer->getJSONData());
	//data.getDynamicObject()->setProperty("automations", automationsManager.getJSONData());

	return data;

}

void LightBlock::loadJSONDataInternal(var data)
{

	ControllableContainer::loadJSONDataInternal(data);

	var pData = data.getProperty("params", var());
	paramsContainer->loadJSONData(pData);
	//automationsManager.loadJSONData(data.getProperty("automations", var()));

	if (paramsContainer->controllables.size() == 0) paramsLoadData = pData; //if params where not already there when loading (using script for exemple), store data to use later
}