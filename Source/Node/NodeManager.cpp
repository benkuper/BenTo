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

juce_ImplementSingleton(NodeFactory)

NodeManager::NodeManager() :
	BaseManager("Nodes"),
	connectionManager(this)
{
	managerFactory = NodeFactory::getInstance();
	
	propNode = new PropNode();
	addItem(propNode);

	prop.resolution->setValue(32);
	addChildControllableContainer(&prop);
}

NodeManager::~NodeManager()
{

}


Array<WeakReference<Controllable>> NodeManager::getExposedParameters()
{
	Array<WeakReference<Controllable>> result;
	for (auto &n : items)
	{
		ParameterNode * pn = dynamic_cast<ParameterNode *>(n);
		if (pn != nullptr) result.add(pn->parameter);
	}

	return result;
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

NodeFactory::NodeFactory()
{
	defs.add(Factory<Node>::Definition::createDef("Color", "Position Remap", &PositionRemapNode::create));
	defs.add(Factory<Node>::Definition::createDef("Color", "Composite", &CompositeNode::create));
	defs.add(Factory<Node>::Definition::createDef("Color", "Model", &ModelNode::create));
	defs.add(Factory<Node>::Definition::createDef("Color", "Color Remap", &ColorRemapNode::create));

	defs.add(Factory<Node>::Definition::createDef("Parameter", "Integer", &ParameterNode::create)->addParam("type", IntParameter::getTypeStringStatic()));
	defs.add(Factory<Node>::Definition::createDef("Parameter", "Float", &ParameterNode::create)->addParam("type", FloatParameter::getTypeStringStatic()));
	defs.add(Factory<Node>::Definition::createDef("Parameter", "Color", &ParameterNode::create)->addParam("type", ColorParameter::getTypeStringStatic()));
	defs.add(Factory<Node>::Definition::createDef("Parameter", "Boolean", &ParameterNode::create)->addParam("type", BoolParameter::getTypeStringStatic()));

}
