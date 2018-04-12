/*
  ==============================================================================

    LightBlock.cpp
    Created: 10 Apr 2018 6:56:47pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlock.h"

#include "LightBlock.h"
#include "ui/LightBlockEditor.h"
#include "Prop/Prop.h"

LightBlock::LightBlock(LightBlockModel * model, Prop * prop, var params) :
	BaseItem(model->niceName),
	prop(prop),
	model(model),
	paramsContainer("Parameters")
{
	addChildControllableContainer(&paramsContainer);

	userCanRemove = false;

	modelRef = model;
	rebuildArgsFromModel();
	model->addCommandModelListener(this);

	startTimerHz(50);
}

LightBlock::~LightBlock()
{
	if (!modelRef.wasObjectDeleted()) model->removeCommandModelListener(this);
}


Array<Colour> LightBlock::getColors()
{
	if (modelRef.wasObjectDeleted()) return Array<Colour>();
	return model->getColors(this);
	
}

void LightBlock::timerCallback()
{
	prop->setColors(model->getColors(this));
}

void LightBlock::rebuildArgsFromModel()
{
	var pData = paramsContainer.getJSONData();

	paramsContainer.clear();

	Array<WeakReference<Parameter>> params = model->getModelParameters();

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

void LightBlock::modelParametersChanged(LightBlockModel *)
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
