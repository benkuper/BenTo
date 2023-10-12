/*
  ==============================================================================

	LightBlockModel.cpp
	Created: 10 Apr 2018 7:12:39pm
	Author:  Ben

  ==============================================================================
*/

LightBlockModel::LightBlockModel(const String &name, var params, ProviderType providerType) :
	LightBlockColorProvider(name, false, false, providerType),
	isBeingEdited(false),
	presetManager(this),
	modelNotifier(5)
{
	setHasCustomColor(true);
	//itemColor->hideInEditor = true;

	itemDataType = "LightBlockModel";
	paramsContainer.reset(new ControllableContainer("Parameters"));
	paramsContainer->saveAndLoadName = false;
	addChildControllableContainer(paramsContainer.get());
	addChildControllableContainer(&presetManager);
}

LightBlockModel::~LightBlockModel()
{
}

void LightBlockModel::clear()
{
	presetManager.clear();

	Array<WeakReference<Controllable>> pList = getModelParameters();
	for (auto &p : pList)
	{
		if (p->type != Controllable::TRIGGER) dynamic_cast<Parameter *>(p.get())->resetValue();
	}
}


void LightBlockModel::setBeingEdited(bool value)
{
	if (value == isBeingEdited) return;
	isBeingEdited = value;
	modelNotifier.addMessage(new ModelEvent(ModelEvent::EDITING_STATE_CHANGED, this));
}

Array<WeakReference<Controllable>> LightBlockModel::getModelParameters()
{
	if (paramsContainer == nullptr) return Array<WeakReference<Controllable>>();
	return paramsContainer->getAllControllables();
}

Array<Colour> LightBlockModel::getColors(Prop * p, double time, var params)
{
	Array<Colour> result;
	result.resize(p->resolution->intValue());
	result.fill(Colours::transparentBlack);

	int id = params.getProperty("forceID", p->globalID->intValue());
	getColorsInternal(&result, p, time, id, p->resolution->intValue(), params);

	return result;
}

void LightBlockModel::setCustomThumbnail(String path)
{
	customThumbnailPath = path;
	modelListeners.call(&ModelListener::customThumbnailChanged, this);
	modelNotifier.addMessage(new ModelEvent(ModelEvent::CUSTOM_THUMBNAIL_CHANGED, this));
}

var LightBlockModel::getJSONData()
{
	var data = LightBlockColorProvider::getJSONData();
	if (paramsContainer != nullptr)
	{
		var cData = paramsContainer->getJSONData();
		if (cData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("modelParams", cData);
	}
	if (presetManager.items.size() > 0) data.getDynamicObject()->setProperty("presets", presetManager.getJSONData());
	return data;
}

void LightBlockModel::loadJSONDataInternal(var data)
{
	LightBlockColorProvider::loadJSONDataInternal(data);
	if (paramsContainer != nullptr) paramsContainer->loadJSONData(data.getProperty("modelParams", var()));
	presetManager.loadJSONData(data.getProperty("presets", var()));
}
LightBlockModelUI * LightBlockModel::createUI()
{
	return new LightBlockModelUI(this);
}
void LightBlockModel::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	if (Engine::mainEngine->isClearing) return;
	if (cc == paramsContainer.get()) providerListeners.call(&ProviderListener::providerParameterValueUpdated, this, dynamic_cast<Parameter *>(c));
}

void LightBlockModel::childStructureChanged(ControllableContainer * cc)
{
	if (cc == paramsContainer.get()) providerListeners.call(&ProviderListener::providerParametersChanged, this);
}
