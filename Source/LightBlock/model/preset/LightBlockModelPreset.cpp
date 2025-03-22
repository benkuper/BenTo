/*
  ==============================================================================

    LightBlockModelPreset.cpp
    Created: 12 Apr 2018 1:10:55pm
    Author:  Ben

  ==============================================================================
*/

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

Array<Colour> LightBlockModelPreset::getColors(Prop * p, double time, var params)
{
	return model->getColors(p, time, params);
}

void LightBlockModelPreset::rebuildArgsFromModel(bool syncValues)
{

	Array<WeakReference<Controllable>> params = model->getModelParameters();

	
	if (params.size() > 0) previousData = paramsContainer.getJSONData();

	paramsContainer.clear();

	for (auto &sc : params)
	{
		if (sc->type == Controllable::TRIGGER)
		{
			paramsContainer.addTrigger(sc->niceName, sc->description);
		} else
		{
			Parameter * p = ControllableFactory::createParameterFrom(sc, true, true);
			if (p != nullptr)
			{
				p->forceSaveValue = true;
				p->setControllableFeedbackOnly(sc->isControllableFeedbackOnly);
				paramsContainer.addParameter(p);
			}
		}

	}

	hideInEditor = controllables.size() == 0;

	paramsContainer.loadJSONData(previousData);
}

void LightBlockModelPreset::childStructureChanged(ControllableContainer * cc)
{
	rebuildArgsFromModel();
}

void LightBlockModelPreset::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	if(cc == &paramsContainer) providerListeners.call(&ColorProviderListener::providerParameterValueUpdated, this, dynamic_cast<Parameter *>(c));
}

var LightBlockModelPreset::getJSONData(bool includeNonOverriden)
{
	var data = BaseItem::getJSONData(includeNonOverriden);
	data.getDynamicObject()->setProperty("params", paramsContainer.getJSONData());
	return data;
}

void LightBlockModelPreset::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	previousData = data.getProperty("params", var());
	paramsContainer.loadJSONData(previousData);
}

Array<WeakReference<Controllable>> LightBlockModelPreset::getModelParameters()
{
	return paramsContainer.getAllControllables();
}
