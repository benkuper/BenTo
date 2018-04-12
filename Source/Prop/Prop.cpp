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

	shape = addEnumParameter("Shape", "The shape of the prop");
	shape->addOption("Club", CLUB)->addOption("Ball", BALL)->addOption("Poi", POI)->addOption("Hoop", HOOP);

	targetModel = addTargetParameter("Target Block", "The current active block for this prop");
	targetModel->targetType = TargetParameter::CONTAINER;
	targetModel->customGetTargetContainerFunc = &LightBlockModelLibrary::showAllModelsAndGet;
}

Prop::~Prop()
{
}

void Prop::setBlockFromModel(LightBlockModel * model)
{
	if (currentBlock == nullptr && model == nullptr) return;
	if (model != nullptr && currentBlock != nullptr && currentBlock->model == model) return;

	if (currentBlock != nullptr)
	{
		removeChildControllableContainer(currentBlock);
		currentBlock = nullptr;
	}

	currentBlock = new LightBlock(model, this);
	

	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
	}

	propListeners.call(&PropListener::propBlockChanged, this);
	propNotifier.addMessage(new PropEvent(PropEvent::BLOCK_CHANGED, this));
}

void Prop::setColors(Array<Colour> newColors)
{
	jassert(newColors.size() == resolution->intValue());
	colors = Array<Colour>(newColors);
	
	propListeners.call(&PropListener::colorsUpdated, this);
	propNotifier.addMessage(new PropEvent(PropEvent::COLORS_UPDATED, this));
}

void Prop::onContainerParameterChanged(Parameter * p)
{
	if (p == targetModel)
	{
		setBlockFromModel(dynamic_cast<LightBlockModel *>(targetModel->targetContainer.get()));
	}
}
