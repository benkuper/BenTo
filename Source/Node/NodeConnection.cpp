/*
  ==============================================================================

	NodeConnection.cpp
	Created: 13 Apr 2018 11:26:22pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeConnection.h"
#include "Node.h"
#include "NodeManager.h"

NodeConnection::NodeConnection(NodeManager * nodeManager) :
	BaseItem("Connection"),
	nodeManager(nodeManager)
{
	miniMode->isSavable = false;
	listUISize->isSavable = false;
	viewUIPosition->isSavable = false;
	viewUISize->isSavable = false;
}

NodeConnection::~NodeConnection()
{
	if (sourceSlot != nullptr && !sourceSlot.wasObjectDeleted()) sourceSlot->removeConnection(this);
	if (destSlot != nullptr && !destSlot.wasObjectDeleted()) destSlot->removeConnection(this);
}

void NodeConnection::dispatchParameterUpdate(Parameter * p)
{
	if (destSlot != nullptr && !destSlot.wasObjectDeleted() && destSlot->type != ConnectionType::ColorBlock)
	{
		((ParameterSlot *)destSlot.get())->handleParameterUpdate(p);
	}
}

void NodeConnection::setSlots(NodeConnectionSlot * source, NodeConnectionSlot * dest)
{
	sourceSlot = source;
	destSlot = dest;
	if (sourceSlot != nullptr) sourceSlot->addConnection(this);
	if (destSlot != nullptr) destSlot->addConnection(this);
	
	if (sourceSlot != nullptr && destSlot != nullptr)
	{
		ParameterSlot * ps = dynamic_cast<ParameterSlot *>(sourceSlot.get());
		if (ps != nullptr)
		{
			dispatchParameterUpdate(ps->parameter);
		}
	}

	setNiceName(sourceSlot->node->niceName + ":" + sourceSlot->name + " > " + destSlot->node->niceName + ":" + destSlot->name);

}

void NodeConnection::insertNode(StringRef nodeType)
{
	connectionListeners.call(&ConnectionListener::askToInsertNode, this, nodeType);
}

var NodeConnection::getJSONData()
{
	var data = BaseItem::getJSONData();
	if (sourceSlot != nullptr && destSlot != nullptr)
	{
		data.getDynamicObject()->setProperty("sourceNode", sourceSlot->node->shortName);
		data.getDynamicObject()->setProperty("sourceSlot", sourceSlot->name);
		data.getDynamicObject()->setProperty("destNode", destSlot->node->shortName);
		data.getDynamicObject()->setProperty("destSlot", destSlot->name);
	}

	return data;
}

void NodeConnection::loadJSONDataInternal(var data)
{
	if (data.hasProperty("sourceSlot"))
	{
		NodeConnectionSlot * source = nodeManager->getSlotForName(data.getProperty("sourceNode", ""), data.getProperty("sourceSlot", ""), false);
		NodeConnectionSlot * dest = nodeManager->getSlotForName(data.getProperty("destNode", ""), data.getProperty("destSlot", ""), true);

		if (source != nullptr && dest != nullptr) setSlots(source, dest);
	}
}
