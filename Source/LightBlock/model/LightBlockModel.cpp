/*
  ==============================================================================

    LightBlockModel.cpp
    Created: 10 Apr 2018 7:12:39pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModel.h"

/*
==============================================================================

LightBlockModel.cpp
Created: 15 Jan 2017 4:53:09pm
Author:  Ben

==============================================================================
*/


#include "LightBlockModel.h"
#include "../LightBlock.h"
#include "Prop/Prop.h"

LightBlockModel::LightBlockModel(const String &name, var params) :
	LightBlockColorProvider(name, false),
	presetManager(this)
{
	itemDataType = "LightBlockModel";
	paramsContainer = new ControllableContainer("Parameters");
	addChildControllableContainer(paramsContainer);
	addChildControllableContainer(&presetManager);
}

LightBlockModel::~LightBlockModel()
{
}

void LightBlockModel::clear()
{
	presetManager.clear();

	Array<WeakReference<Parameter>> pList = getModelParameters();
	for (auto &p : pList) p->resetValue();
}

Array<WeakReference<Parameter>> LightBlockModel::getModelParameters()
{
	if (paramsContainer == nullptr) return Array<WeakReference<Parameter>>();
	return paramsContainer->getAllParameters();
}

void LightBlockModel::updateColorsForBlock(LightBlock * block, var params)
{
	//To be overriden by child classes
	int numLeds = block->prop->resolution->intValue();
	for (int i = 0; i < numLeds; i++)
	{
		block->prop->colors.set(i, Colours::black);
	}
}

var LightBlockModel::getJSONData()
{
	var data = BaseItem::getJSONData();
	if(paramsContainer != nullptr) data.getDynamicObject()->setProperty("parameters", paramsContainer->getJSONData());
	data.getDynamicObject()->setProperty("presets", presetManager.getJSONData());
	return data;
}

void LightBlockModel::loadJSONDataInternal(var data)
{
	if(paramsContainer != nullptr) paramsContainer->loadJSONData(data.getProperty("parameters", var()));
	presetManager.loadJSONData(data.getProperty("presets", var()));
}
void LightBlockModel::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *)
{
	if (cc == paramsContainer) providerListeners.call(&ProviderListener::providerParametersChanged, this);
}

void LightBlockModel::childStructureChanged(ControllableContainer * cc)
{
	if (cc == paramsContainer) providerListeners.call(&ProviderListener::providerParametersChanged, this);
}
