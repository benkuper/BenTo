/*
  ==============================================================================

	NodeConnectionManager.cpp
	Created: 13 Apr 2018 11:26:06pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeConnectionManager.h"
#include "Node.h"
#include "NodeManager.h"

NodeConnectionManager::NodeConnectionManager(NodeManager * nodeManager) :
	BaseManager("Connections"),
	nodeManager(nodeManager)
{
	userCanAddItemsManually = false;
	isSelectable = false;
}

NodeConnectionManager::~NodeConnectionManager()
{
}

NodeConnection * NodeConnectionManager::createItem()
{
	return new NodeConnection(nodeManager);
}

void NodeConnectionManager::addConnection(NodeConnectionSlot * source, NodeConnectionSlot * dest)
{
	NodeConnection * nc = getConnectionForSlots(source, dest);
	if (nc != nullptr) return;
	nc = createItem();
	nc->setSlots(source, dest);
	addItem(nc);
}

NodeConnection * NodeConnectionManager::getConnectionForSlots(NodeConnectionSlot * source, NodeConnectionSlot * dest)
{
	for (auto & c : items) if (c->sourceSlot == source && c->destSlot == dest) return c;
	return nullptr;
}
