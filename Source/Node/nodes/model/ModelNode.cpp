/*
  ==============================================================================

	ModelNode.cpp
	Created: 13 Apr 2018 11:30:06pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "Node/NodeIncludes.h"
#include "ParameterLink/ParameterLink.h"

ModelNode::ModelNode(var params) :
	ColorNode(getTypeString(), params),
	currentBlock(nullptr)
{
	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showSourcesAndGet;
}

ModelNode::~ModelNode()
{
}


void ModelNode::setBlockFromProvider(LightBlockColorProvider* provider)
{
	if (currentBlock == nullptr && provider == nullptr) return;
	if (provider != nullptr && currentBlock != nullptr && currentBlock->provider == provider) return;

	while (inSlots.size() > 0) removeSlot(true, inSlots[0]);

	if (currentBlock != nullptr)
	{
		removeChildControllableContainer(currentBlock.get());
		currentBlock = nullptr;
	}

	if (provider != nullptr) currentBlock.reset(new LightBlock(provider));


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock.get());

		LightBlockModelPreset* preset = dynamic_cast<LightBlockModelPreset*>(provider);
		if (preset == nullptr)
		{
			Array<WeakReference<Parameter>> blockParams = currentBlock->getAllParameters(true);
			for (auto& p : blockParams) p->isOverriden = true;
		}
	}

	buildSlots();
}

void ModelNode::buildSlots()
{
	if (currentBlock == nullptr) return;

	Array<WeakReference<Parameter>> blockParams = currentBlock->paramsContainer->getAllParameters();
	for (auto& p : blockParams)
	{
		if (p->type == Controllable::ENUM || p->type == Controllable::TARGET || p->type == Controllable::POINT2D || p->type == Controllable::POINT3D) continue;

		addParameterSlot(true, p);
	}
}
Array<Colour> ModelNode::getColorsInternal(Prop* p, double time, var params, var localParams)
{
	if (currentBlock == nullptr) return ColorNode::getColorsInternal(p, time, params, localParams);
	return currentBlock->getColors(p, time, localParams);
}

void ModelNode::onContainerParameterChanged(Parameter* p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider*>(activeProvider->targetContainer.get()));
	}
}


var ModelNode::getJSONData(bool includeNonOverriden)
{
	var data = ColorNode::getJSONData(includeNonOverriden);
	if (currentBlock != nullptr) data.getDynamicObject()->setProperty("block", currentBlock->getJSONData());
	return data;
}

void ModelNode::loadJSONDataInternal(var data)
{
	ColorNode::loadJSONDataInternal(data);
	if (currentBlock != nullptr) currentBlock->loadJSONData(data.getProperty("block", var()));
}


NodeViewUI* ModelNode::createUI()
{
	return new ModelNodeUI(this);
}
