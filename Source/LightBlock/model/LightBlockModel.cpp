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
#include "Prop/Prop.h"

LightBlockModel::LightBlockModel(const String &name, var params) :
	BaseItem(name, false)
{
	itemDataType = "LightBlockModel";
	addChildControllableContainer(&parameters);
	parameters.addBaseManagerListener(this); 

}

LightBlockModel::~LightBlockModel()
{
	parameters.removeBaseManagerListener(this);
}

Array<Colour> LightBlockModel::getColours(Prop * prop, var params)
{
	//default behavior, must be overriden by child classes

	int numColors = prop->resolution->intValue();
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
	data.getDynamicObject()->setProperty("arguments", parameters.getJSONData());
	return data;
}

void LightBlockModel::loadJSONDataInternal(var data)
{
	DBG("Load jsondata internal " << niceName);
	parameters.loadJSONData(data.getProperty("arguments", var()));
}

void LightBlockModel::onContainerParameterChangedInternal(Parameter * p)
{

}

void LightBlockModel::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	if (cc == &parameters) modelListeners.call(&ModelListener::modelParametersChanged, this);
}

void LightBlockModel::itemAdded(LightBlockModelParameter *)
{
	modelListeners.call(&ModelListener::modelParametersChanged, this);
}

void LightBlockModel::itemRemoved(LightBlockModelParameter *)
{
	modelListeners.call(&ModelListener::modelParametersChanged, this);
}

void LightBlockModel::itemsReordered()
{
	modelListeners.call(&ModelListener::modelParametersChanged, this);
}

