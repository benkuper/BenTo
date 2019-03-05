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

void NodeConnectionManager::addItemInternal(NodeConnection * nc, var)
{
	nc->addConnectionListener(this);
}

void NodeConnectionManager::removeItemInternal(NodeConnection * nc)
{
	nc->removeConnectionListener(this);
}

void NodeConnectionManager::askToInsertNode(NodeConnection * nc, StringRef nodeType)
{
	Node * node = nodeManager->addItem(NodeFactory::getInstance()->create(nodeType));
	if (node == nullptr)
	{
		DBG("Can't create node of type : " << nodeType);
		return;
	}

	node->viewUIPosition->setPoint((nc->sourceSlot->node->viewUIPosition->getPoint() + nc->destSlot->node->viewUIPosition->getPoint()) / 2);

	NodeConnectionSlot * n1 = nc->sourceSlot;
	NodeConnectionSlot * n2 = nc->destSlot;

	Array<NodeConnectionSlot *> inSlots = node->getSlotsWithType(true, n1->type);
	Array<NodeConnectionSlot *> outSlots = node->getSlotsWithType(false, n2->type);

	if (inSlots.size() == 0 || outSlots.size() == 0)
	{
		LOGWARNING("No matching connections found");
		return;
	}

	removeItem(nc);
	addConnection(n1, inSlots[0]);
	addConnection(outSlots[0], n2);
}
