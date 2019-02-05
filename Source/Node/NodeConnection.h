/*
  ==============================================================================

    NodeConnection.h
    Created: 13 Apr 2018 11:26:22pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "NodeConnectionSlot.h"
class NodeManager;

class NodeConnection :
	public BaseItem
{
public:
	NodeConnection(NodeManager * manager = nullptr);
	~NodeConnection();

	WeakReference<NodeConnectionSlot> sourceSlot;
	WeakReference<NodeConnectionSlot> destSlot;

	NodeManager * nodeManager;
	ConnectionType type;

	void dispatchParameterUpdate(Parameter *p);

	void setSlots(NodeConnectionSlot * source, NodeConnectionSlot * dest);

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
};