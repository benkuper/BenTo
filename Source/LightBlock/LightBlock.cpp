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

LightBlock::LightBlock(LightBlockModel * model, var params) :
	BaseItem(model->niceName),
	model(model),
	paramsContainer("Parameters")
{
	addChildControllableContainer(&paramsContainer);

	modelRef = model;
	rebuildArgsFromModel();
	model->addCommandModelListener(this);
}

LightBlock::~LightBlock()
{
	if (!modelRef.wasObjectDeleted()) model->removeCommandModelListener(this);
}

void LightBlock::rebuildArgsFromModel()
{
	var data = getJSONData();

	paramsContainer.clear();

	//TODO : SlaveParameter ?
	Parameter * p = nullptr;
	for (auto &a : model->parameters.items)
	{
		Parameter * ap = a->param;
		switch (ap->type)
		{
		case Controllable::BOOL: p = new BoolParameter(a->niceName, ap->description, ap->getValue()); break;
		case Controllable::INT: p = new IntParameter(a->niceName, ap->description, ap->getValue(), ap->minimumValue, ap->maximumValue); break;
		case Controllable::FLOAT: p = new FloatParameter(a->niceName, ap->description, ap->getValue(), ap->minimumValue, ap->maximumValue); break;
		case Controllable::STRING: p = new StringParameter(a->niceName, ap->description, ap->getValue()); break;
		default:
			break;
		}

		paramsContainer.addParameter(p);
		p->isEditable = a->editable->boolValue();
	}

	paramsContainer.hideInEditor = model->parameters.items.size() == 0;

	loadJSONData(data);
}

void LightBlock::modelParametersChanged(LightBlockModel *)
{
	rebuildArgsFromModel();
}
