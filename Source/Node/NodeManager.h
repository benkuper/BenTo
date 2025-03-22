/*
  ==============================================================================

    NodeManager.h
    Created: 13 Apr 2018 11:25:07pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Node.h"
#include "NodeConnectionManager.h"

class Prop;
class PropNode;

class NodeFactory :
	public Factory<Node>
{
public:
	juce_DeclareSingleton(NodeFactory, true)
		
	NodeFactory();
	~NodeFactory() {}
};


class NodeManager :
	public BaseManager<Node>
{
public:
	NodeManager();
	~NodeManager();

	
	PropNode * propNode;
	std::unique_ptr<Prop> prop;

	NodeConnectionManager connectionManager;
	Array<WeakReference<Controllable>> getExposedParameters();

	NodeConnectionSlot * getSlotForName(const String &nodeName, const String &slotName, bool isInput);

	var getJSONData(bool includeNonOverriden = false) override;
	void loadJSONDataManagerInternal(var data) override;

};