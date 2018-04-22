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
	paramsContainer("Parameters")
{
	addChildControllableContainer(&paramsContainer);

	userCanRemove = false;

	rebuildArgsFromModel();
	provider->addColorProviderListener(this);

}


LightBlock::~LightBlock()
{
	if (!provider.wasObjectDeleted()) provider->removeColorProviderListener(this);
}

Array<Colour> LightBlock::getColors(int id, int resolution, float time, var params)
{
	if (params.isVoid()) params = new DynamicObject();
	Array<WeakReference<Parameter>> paramList = paramsContainer.getAllParameters();
	for (auto &p : paramList)
	{
		if (p.wasObjectDeleted() || p == nullptr) continue;
		params.getDynamicObject()->setProperty(p->shortName, p->value);
	}
	if (provider.wasObjectDeleted())
	{
		Array<Colour> result;
		result.resize(resolution);
		result.fill(Colours::black);
		return result;
	}

	return provider->getColors(id, resolution, time, params);
}

void LightBlock::rebuildArgsFromModel()
{
	var pData = paramsContainer.getJSONData();

	paramsContainer.clear();

	Array<WeakReference<Parameter>> params = provider->getModelParameters();

	for (auto &sp : params)
	{
		Parameter * p = nullptr;
		switch (sp->type)
		{
		case Controllable::BOOL: p = new BoolParameter(sp->niceName, sp->description, sp->getValue()); break;
		case Controllable::INT: p = new IntParameter(sp->niceName, sp->description, sp->getValue(), sp->minimumValue, sp->maximumValue); break;
		case Controllable::FLOAT: p = new FloatParameter(sp->niceName, sp->description, sp->getValue(), sp->minimumValue, sp->maximumValue); break;
		case Controllable::STRING: p = new StringParameter(sp->niceName, sp->description, sp->getValue()); break;
		case Controllable::COLOR: p = new ColorParameter(sp->niceName, sp->description, ((ColorParameter *)sp.get())->getColor()); break;
		default:
			break;
		}

		if (p != nullptr)
		{
			p->setControllableFeedbackOnly(sp->isControllableFeedbackOnly);
			paramsContainer.addParameter(p);
		}
		
	}

	paramsContainer.hideInEditor = paramsContainer.controllables.size() == 0;
	paramsContainer.loadJSONData(pData);
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
	return data;

}

void LightBlock::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	paramsContainer.loadJSONData(data.getProperty("params", var()));
}
