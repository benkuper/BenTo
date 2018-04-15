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

LightBlock::LightBlock(LightBlockColorProvider * provider, Prop * prop, var params) :
	BaseItem(provider->niceName),
	prop(prop),
	provider(provider),
	paramsContainer("Parameters")
{
	addChildControllableContainer(&paramsContainer);

	userCanRemove = false;

	rebuildArgsFromModel();
	provider->addColorProviderListener(this);

	startTimerHz(50);
}

LightBlock::~LightBlock()
{
	if (!provider.wasObjectDeleted()) provider->removeColorProviderListener(this);
}

void LightBlock::update()
{
	provider->updateColorsForBlock(this);
	blockListeners.call(&LightBlockListener::colorsUpdated);
}

void LightBlock::timerCallback()
{
	if (provider.wasObjectDeleted())
	{
		DBG("Provider has been deleted !");
		stopTimer();
		return;
	}

	update();
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
