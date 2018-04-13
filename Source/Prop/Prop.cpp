/*
  ==============================================================================

	Prop.cpp
	Created: 10 Apr 2018 6:59:25pm
	Author:  Ben

  ==============================================================================
*/

#include "Prop.h"
#include "LightBlock/model//LightBlockModelLibrary.h"

Prop::Prop(const String &name, var) :
	BaseItem(name),
	propNotifier(50),
	currentBlock(nullptr)
{
	id = addIntParameter("ID", "Prop ID", 0, 0, 100);
	resolution = addIntParameter("Resolution", "Number of controllable colors in the prop", 1, 1, INT32_MAX);
	colors.resize(resolution->intValue());

	shape = addEnumParameter("Shape", "The shape of the prop");
	shape->addOption("Club", CLUB)->addOption("Ball", BALL)->addOption("Poi", POI)->addOption("Hoop", HOOP);

	activeProvider= addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showProvidersAndGet;
}

Prop::~Prop()
{
	setBlockFromProvider(nullptr);
}

void Prop::setBlockFromProvider(LightBlockColorProvider * model)
{
	if (currentBlock == nullptr && model == nullptr) return;
	if (model != nullptr && currentBlock != nullptr && currentBlock->provider == model) return;

	if (currentBlock != nullptr)
	{
		removeChildControllableContainer(currentBlock);
		if(!currentBlock->provider.wasObjectDeleted()) currentBlock->provider->removeInspectableListener(this);
		currentBlock->removeLightBlockListener(this);
		currentBlock = nullptr;
	}

	if(model != nullptr) currentBlock = new LightBlock(model, this);
	

	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
		currentBlock->provider->addInspectableListener(this);
		currentBlock->addLightBlockListener(this);
	}

	propListeners.call(&PropListener::propBlockChanged, this);
	propNotifier.addMessage(new PropEvent(PropEvent::BLOCK_CHANGED, this));
}

void Prop::onContainerParameterChanged(Parameter * p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	} else if (p == resolution)
	{
		colors.resize(resolution->intValue());
	}
}

void Prop::inspectableDestroyed(Inspectable * i)
{
	if (currentBlock != nullptr && i == currentBlock->provider) setBlockFromProvider(nullptr);
}

void Prop::colorsUpdated()
{
	propListeners.call(&PropListener::colorsUpdated, this);
	propNotifier.addMessage(new PropEvent(PropEvent::COLORS_UPDATED, this));
}

var Prop::getJSONData()
{
	var data = BaseItem::getJSONData();
	if (currentBlock != nullptr) data.getDynamicObject()->setProperty("block", currentBlock->getJSONData());
	return data;
}

void Prop::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	if (currentBlock != nullptr) currentBlock->loadJSONData(data.getProperty("block", var()));
}
