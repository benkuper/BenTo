/*
  ==============================================================================

	ModelNode.cpp
	Created: 13 Apr 2018 11:30:06pm
	Author:  Ben

  ==============================================================================
*/

#include "ModelNode.h"
#include "ui/ModelNodeUI.h"
#include "LightBlock/LightBlock.h"
#include "LightBlock/model/LightBlockModelLibrary.h"
#include "Prop/Prop.h"
#include "Node/nodes/parameter/ParameterNode.h"

ModelNode::ModelNode(var params) :
	ColorNode(getTypeString(), params),
	currentBlock(nullptr)
{
	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showProvidersAndGet;
}

ModelNode::~ModelNode()
{
}


void ModelNode::setBlockFromProvider(LightBlockColorProvider * provider)
{
	if (currentBlock == nullptr && provider == nullptr) return;
	if (provider != nullptr && currentBlock != nullptr && currentBlock->provider == provider) return;

	while (inSlots.size() > 0) removeSlot(true, inSlots[0]); 
	
	if (currentBlock != nullptr)
	{
		removeChildControllableContainer(currentBlock);
		currentBlock = nullptr;
	}

	if (provider != nullptr) currentBlock = new LightBlock(provider);


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
	}

	buildSlots();
}

void ModelNode::buildSlots()
{
	if (currentBlock == nullptr) return;
	
	Array<WeakReference<Parameter>> blockParams = currentBlock->paramsContainer.getAllParameters();
	for (auto &p : blockParams)
	{
		if (p->type == Controllable::ENUM || p->type == Controllable::TARGET || p->type == Controllable::POINT2D || p->type == Controllable::POINT3D) continue;
		
		addParameterSlot(true, p);
	}
} 
Array<Colour> ModelNode::getColors(Prop * p, double time, var params)
{
	if (currentBlock == nullptr) return ColorNode::getColorsInternal(p, time, params);

	fillWithLocalParams(params);
	return currentBlock->getColors(p, time, params);
}

void ModelNode::onContainerParameterChanged(Parameter * p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	}
}


var ModelNode::getJSONData()
{
	var data = ColorNode::getJSONData();
	if (currentBlock != nullptr) data.getDynamicObject()->setProperty("block", currentBlock->getJSONData());
	return data;
}

void ModelNode::loadJSONDataInternal(var data)
{
	ColorNode::loadJSONDataInternal(data);
	if (currentBlock != nullptr) currentBlock->loadJSONData(data.getProperty("block", var()));
}


NodeViewUI * ModelNode::createUI()
{
	return new ModelNodeUI(this);
}
