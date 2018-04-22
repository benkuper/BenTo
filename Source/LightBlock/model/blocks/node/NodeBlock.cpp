/*
  ==============================================================================

	NodeBlock.cpp
	Created: 10 Apr 2018 6:59:08pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeBlock.h"
#include "Node/nodes/prop/PropNode.h"

NodeBlock::NodeBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	addChildControllableContainer(&manager);
}

NodeBlock::~NodeBlock()
{
}

Array<Colour> NodeBlock::getColors(int id, int resolution, float time, var params)
{
	return  manager.propNode->getColors(id, resolution, time, params);
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
