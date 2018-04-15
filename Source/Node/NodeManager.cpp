/*
  ==============================================================================

	NodeManager.cpp
	Created: 13 Apr 2018 11:25:07pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeManager.h"
#include "nodes/colorRemap/ColorRemapNode.h"
#include "nodes/composite/CompositeNode.h"
#include "nodes/model/ModelNode.h"
#include "nodes/parameter/ParameterNode.h"
#include "nodes/positionRemap/PositionRemapNode.h"
#include "nodes/prop/PropNode.h"
#include "nodes/solidcolor/SolidColorNode.h"

NodeManager::NodeManager() :
	BaseManager("Nodes"),
	connectionManager(this)
{
	managerFactory = &factory;
	factory.defs.add(Factory<Node>::Definition::createDef("Color", "Solid Color", &SolidColorNode::create));
	factory.defs.add(Factory<Node>::Definition::createDef("Color", "Position Remap", &PositionRemapNode::create));
	factory.defs.add(Factory<Node>::Definition::createDef("Color", "Composite", &CompositeNode::create));
	factory.defs.add(Factory<Node>::Definition::createDef("Color", "Model", &ModelNode::create));
	factory.defs.add(Factory<Node>::Definition::createDef("Color", "Color Remap", &ColorRemapNode::create));
	
	factory.defs.add(Factory<Node>::Definition::createDef("Parameter", "Integer", &ParameterNode::create)->addParam("type",IntParameter::getTypeStringStatic()));
	factory.defs.add(Factory<Node>::Definition::createDef("Parameter", "Float", &ParameterNode::create)->addParam("type", FloatParameter::getTypeStringStatic()));
	factory.defs.add(Factory<Node>::Definition::createDef("Parameter", "Color", &ParameterNode::create)->addParam("type", ColorParameter::getTypeStringStatic()));
	factory.defs.add(Factory<Node>::Definition::createDef("Parameter", "Boolean", &ParameterNode::create)->addParam("type", BoolParameter::getTypeStringStatic()));

	propNode = new PropNode();
	addItem(propNode);

	prop.resolution->setValue(32);
	addChildControllableContainer(&prop);
}

NodeManager::~NodeManager()
{

}


NodeConnectionSlot * NodeManager::getSlotForName(const String & nodeName, const String & slotName, bool isInput)
{
	Node * n = getItemWithName(nodeName);
	if (n == nullptr) return nullptr;
	return n->getSlotWithName(isInput, slotName);
}

var NodeManager::getJSONData()
{
	var data = BaseManager::getJSONData();
	data.getDynamicObject()->setProperty("connections", connectionManager.getJSONData());
	data.getDynamicObject()->setProperty("prop", propNode->getJSONData());

	return data;
}

void NodeManager::loadJSONDataManagerInternal(var data)
{
	//after clear, must recreate the prop node

	propNode = new PropNode();
	addItem(propNode);
	propNode->loadJSONData(data.getProperty("prop",var()));

	BaseManager::loadJSONDataManagerInternal(data);

	connectionManager.loadJSONData(data.getProperty("connections", var()));
}
