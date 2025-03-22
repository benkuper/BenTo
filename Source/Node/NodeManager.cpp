/*
  ==============================================================================

	NodeManager.cpp
	Created: 13 Apr 2018 11:25:07pm
	Author:  Ben

  ==============================================================================
*/

#include "Node/NodeIncludes.h"

juce_ImplementSingleton(NodeFactory)

NodeManager::NodeManager() :
	BaseManager("Nodes"),
	connectionManager(this)
{
	managerFactory = NodeFactory::getInstance();

	propNode = new PropNode();
	addItem(propNode);

	prop.reset(new Prop());
	prop->resolution->setValue(32);
	addChildControllableContainer(prop.get());
}

NodeManager::~NodeManager()
{

}


Array<WeakReference<Controllable>> NodeManager::getExposedParameters()
{
	Array<WeakReference<Controllable>> result;
	for (auto& n : items)
	{
		ParameterNode* pn = dynamic_cast<ParameterNode*>(n);
		if (pn != nullptr) result.add(pn->parameter);
	}

	return result;
}

NodeConnectionSlot* NodeManager::getSlotForName(const String& nodeName, const String& slotName, bool isInput)
{
	Node* n = getItemWithName(nodeName);
	if (n == nullptr) return nullptr;
	return n->getSlotWithName(isInput, slotName);
}

var NodeManager::getJSONData(bool includeNonOverriden)
{
	var data = BaseManager::getJSONData(includeNonOverriden);
	data.getDynamicObject()->setProperty("connections", connectionManager.getJSONData());
	data.getDynamicObject()->setProperty("prop", propNode->getJSONData());

	return data;
}

void NodeManager::loadJSONDataManagerInternal(var data)
{
	//after clear, must recreate the prop node

	propNode = new PropNode();
	addItem(propNode);
	propNode->loadJSONData(data.getProperty("prop", var()));

	BaseManager::loadJSONDataManagerInternal(data);

	connectionManager.loadJSONData(data.getProperty("connections", var()));
}

NodeFactory::NodeFactory()
{
	defs.add(Factory<Node>::Definition::createDef<ModelNode>("Color")->addParam("nodeType", "color"));
	defs.add(Factory<Node>::Definition::createDef<CompositeNode>("Color")->addParam("nodeType", "color"));
	defs.add(Factory<Node>::Definition::createDef<PositionRemapNode>("Color")->addParam("nodeType", "color"));
	defs.add(Factory<Node>::Definition::createDef<ColorRemapNode>("Color")->addParam("nodeType", "color"));
	defs.add(Factory<Node>::Definition::createDef<SplitNode>("Color")->addParam("nodeType", "color"));

	defs.add(Factory<Node>::Definition::createDef("Parameter", "Integer", &ParameterNode::create)->addParam("nodeType", "param")->addParam("type", IntParameter::getTypeStringStatic()));
	defs.add(Factory<Node>::Definition::createDef("Parameter", "Float", &ParameterNode::create)->addParam("nodeType", "param")->addParam("type", FloatParameter::getTypeStringStatic()));
	defs.add(Factory<Node>::Definition::createDef("Parameter", "Color", &ParameterNode::create)->addParam("nodeType", "param")->addParam("type", ColorParameter::getTypeStringStatic()));
	defs.add(Factory<Node>::Definition::createDef("Parameter", "Boolean", &ParameterNode::create)->addParam("nodeType", "param")->addParam("type", BoolParameter::getTypeStringStatic()));

}
