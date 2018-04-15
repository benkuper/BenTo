/*
  ==============================================================================

    NodeConnectionManager.h
    Created: 13 Apr 2018 11:26:06pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "NodeConnection.h"
class NodeManager;

class NodeConnectionManager :
	public BaseManager<NodeConnection>
{
public:
	NodeConnectionManager(NodeManager * manager);
	~NodeConnectionManager();

	NodeManager * nodeManager;

	NodeConnection * createItem() override;

	void addConnection(NodeConnectionSlot * source, NodeConnectionSlot * dest);
	NodeConnection * getConnectionForSlots(NodeConnectionSlot * source, NodeConnectionSlot * dest);
};