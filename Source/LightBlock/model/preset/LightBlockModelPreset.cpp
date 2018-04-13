/*
  ==============================================================================

    LightBlockModelPreset.cpp
    Created: 12 Apr 2018 1:10:55pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelPreset.h"
#include "../LightBlockModel.h"

LightBlockModelPreset::LightBlockModelPreset(LightBlockModel * model, var) :
	LightBlockColorProvider("Preset", false),
	model(model),
	modelRef(model),
	paramsContainer("Parameters")
{
	isSelectable = false;
	model->addControllableContainerListener(this);
	rebuildArgsFromModel();
	
	paramsContainer.editorCanBeCollapsed = false;
	paramsContainer.hideEditorHeader = true;
	addChildControllableContainer(&paramsContainer);
}

LightBlockModelPreset::~LightBlockModelPreset()
{
	if (!modelRef.wasObjectDeleted()) model->removeControllableContainerListener(this);
}

void LightBlockModelPreset::updateColorsForBlock(LightBlock * b, var params)
{
	model->updateColorsForBlock(b, params);
}

void LightBlockModelPreset::rebuildArgsFromModel(bool syncValues)
{

	Array<WeakReference<Parameter>> params = model->getModelParameters();

	if (params.size() > 0) previousData = paramsContainer.getJSONData();

	paramsContainer.clear();

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
			p->forceSaveValue = true;
			p->setControllableFeedbackOnly(sp->isControllableFeedbackOnly);
			paramsContainer.addParameter(p);
		}
	}

	hideInEditor = controllables.size() == 0;

	paramsContainer.loadJSONData(previousData);
}

void LightBlockModelPreset::childStructureChanged(ControllableContainer * cc)
{
	rebuildArgsFromModel();
}

void LightBlockModelPreset::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *)
{
	if(cc == &paramsContainer) providerListeners.call(&ProviderListener::providerParametersChanged, this);
}

var LightBlockModelPreset::getJSONData()
{
	var data = BaseItem::getJSONData();
	data.getDynamicObject()->setProperty("params", paramsContainer.getJSONData());
	return data;
}

void LightBlockModelPreset::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	previousData = data.getProperty("params", var());
	paramsContainer.loadJSONData(previousData);
}

Array<WeakReference<Parameter>> LightBlockModelPreset::getModelParameters()
{
	return paramsContainer.getAllParameters();
}
