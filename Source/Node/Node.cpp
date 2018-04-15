/*
  ==============================================================================

    Node.cpp
    Created: 14 Apr 2018 2:11:31am
    Author:  Ben

  ==============================================================================
*/

#include "Node.h"
#include "ui/NodeViewUI.h"
#include "nodes/parameter/ParameterNode.h"
#include "ColorNode.h"

Node::Node(const String & name, var) :
	BaseItem(name),
	nodeNotifier(10)
{

}

Node::~Node()
{
	masterReference.clear();
}

ParameterSlot * Node::addParameterSlot(bool isInput, Parameter * p)
{
	if (p->type != Parameter::FLOAT && p->type != Parameter::INT && p->type != Parameter::COLOR)
	{
		DBG("Other types are not supported !");
	}

	ParameterSlot * s = static_cast<ParameterSlot *>(getSlotWithName(isInput, p->niceName));
	if (s == nullptr)
	{
		s = new ParameterSlot(this, isInput, p);
		if (isInput) inSlots.add(s);
		else outSlots.add(s);
		nodeListeners.call(&NodeListener::slotAdded, this, s, isInput);
		nodeNotifier.addMessage(new NodeEvent(isInput ? NodeEvent::INSLOT_ADDED : NodeEvent::OUTSLOT_ADDED, this, s, s->name));
	}

	return s;
}

ColorSlot * Node::addColorSlot(bool isInput, const String & name)
{
	ColorSlot * s = static_cast<ColorSlot *>(getSlotWithName(isInput, name));
	if (s == nullptr)
	{
		s = new ColorSlot(this, isInput, name);
		if (isInput) inSlots.add(s);
		else outSlots.add(s);
		nodeListeners.call(&NodeListener::slotAdded, this, s, isInput);
		nodeNotifier.addMessage(new NodeEvent(isInput ? NodeEvent::INSLOT_ADDED : NodeEvent::OUTSLOT_ADDED, this, s, s->name));
	}
	return s;
}

void Node::removeSlot(bool isInput, const String & name)
{
	NodeConnectionSlot * s = getSlotWithName(isInput, name);
	removeSlot(isInput, s);
}

void Node::removeSlot(bool isInput, NodeConnectionSlot * s)
{
	if (s != nullptr)
	{
		if (isInput) inSlots.removeObject(s, false);
		else outSlots.removeObject(s, false);
		nodeListeners.call(&NodeListener::slotRemoved, this, s, isInput);
		nodeNotifier.addMessage(new NodeEvent(isInput ? NodeEvent::INSLOT_REMOVED : NodeEvent::OUTSLOT_REMOVED, this, s, s->name));
		delete s;
	}
}

NodeConnectionSlot * Node::getSlotWithName(bool isInput, const String & name)
{
	if (isInput)
	{
		for (auto & s : inSlots) if (s->name == name) return s;
	} else {
		for (auto & s : outSlots) if (s->name == name) return s;
	}

	return nullptr;
}

NodeViewUI * Node::createUI()
{
	return new NodeViewUI(this);
}