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
	BaseItem(name, false)
{
	itemDataType = "LightBlockModel";
	paramsContainer = new ControllableContainer("Parameters");
	addChildControllableContainer(paramsContainer);
}

LightBlockModel::~LightBlockModel()
{
}

Array<WeakReference<Parameter>> LightBlockModel::getModelParameters()
{
	if (paramsContainer == nullptr) return Array<WeakReference<Parameter>>();
	return paramsContainer->getAllParameters();
}


Array<Colour> LightBlockModel::getColors(LightBlock * block, var params)
{
	//default behavior, must be overriden by child classes
	int numColors = block->prop->resolution->intValue();
	Array<Colour> result;
	for (int i = 0; i < numColors; i++)
	{
		result.add(Colours::black);
	}

	return result;
}

var LightBlockModel::getJSONData()
{
	var data = BaseItem::getJSONData();
	if(paramsContainer != nullptr) data.getDynamicObject()->setProperty("parameters", paramsContainer->getJSONData());
	return data;
}

void LightBlockModel::loadJSONDataInternal(var data)
{
	DBG("Load jsondata internal " << niceName);
	if(paramsContainer != nullptr) paramsContainer->loadJSONData(data.getProperty("parameters", var()));
}

void LightBlockModel::onContainerParameterChangedInternal(Parameter * p)
{

}

void LightBlockModel::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	if (cc == paramsContainer) modelListeners.call(&ModelListener::modelParametersChanged, this);
}

void LightBlockModel::childStructureChanged(ControllableContainer * cc)
{
	if (cc == paramsContainer) modelListeners.call(&ModelListener::modelParametersChanged, this);
}
