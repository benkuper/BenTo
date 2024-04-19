/*
  ==============================================================================

    NodeConnectionManager.h
    Created: 13 Apr 2018 11:26:06pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
class NodeManager;

class NodeConnectionManager :
	public BaseManager<NodeConnection>,
	public NodeConnection::ConnectionListener
{
public:
	NodeConnectionManager(NodeManager * manager);
	~NodeConnectionManager();

	NodeManager * nodeManager;

	NodeConnection * createItem() override;

	void addConnection(NodeConnectionSlot * source, NodeConnectionSlot * dest);
	NodeConnection * getConnectionForSlots(NodeConnectionSlot * source, NodeConnectionSlot * dest);

	void addItemInternal(NodeConnection* nc, var) override;
	void addItemsInternal(Array<NodeConnection *> items, var) override;
	void removeItemInternal(NodeConnection* nc) override;
	void removeItemsInternal(Array<NodeConnection *> items) override;

	void askToInsertNode(NodeConnection * nc, StringRef nodeType) override;
};