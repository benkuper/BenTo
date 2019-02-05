/*
  ==============================================================================

	NodeBlock.cpp
	Created: 10 Apr 2018 6:59:08pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeBlock.h"
#include "Node/nodes/prop/PropNode.h"
#include "ui/NodeBlockUI.h"

NodeBlock::NodeBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	addChildControllableContainer(&manager);
}

NodeBlock::~NodeBlock()
{
}

Array<Colour> NodeBlock::getColors(Prop * p, double time, var params)
{
	return  manager.propNode->getColors(p, time, params);
}

var NodeBlock::getJSONData()
{
	var data = LightBlockModel::getJSONData();
	data.getDynamicObject()->setProperty("nodes", manager.getJSONData());
	return data;
}

void NodeBlock::loadJSONDataInternal(var data)
{
	LightBlockModel::loadJSONDataInternal(data);
	manager.loadJSONDataInternal(data.getProperty("nodes", ""));
}

LightBlockModelUI * NodeBlock::createUI()
{
	return new NodeBlockUI(this);
}
