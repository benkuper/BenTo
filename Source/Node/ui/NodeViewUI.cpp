/*
  ==============================================================================

	NodeViewUI.cpp
	Created: 13 Apr 2018 11:25:28pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeViewUI.h"
#include "NodeConnectionUI.h"

NodeViewUI::NodeViewUI(Node * node, Direction direction) :
	BaseItemUI(node, direction, Direction::ALL)
{
	for (auto & s : node->inSlots) addConnector(true, s, false);
	for (auto & s : node->outSlots) addConnector(false, s, false);
	
	node->addAsynNodeListener(this);
	showEnableBT = false;
}

NodeViewUI::~NodeViewUI()
{
	if (!inspectable.wasObjectDeleted()) item->removeAsyncNodeListener(this);
}


void NodeViewUI::resized()
{
	BaseItemUI::resized();

	//place connectors

	Rectangle<int> lr = getLocalBounds().removeFromLeft(connectorSize);
	Rectangle<int> rr = getLocalBounds().removeFromRight(connectorSize);

	lr.removeFromTop(20);
	rr.removeFromTop(20);

	for (auto & c : inConnectors)
	{
		c->setBounds(lr.removeFromTop(connectorSize));
		lr.removeFromTop(10);
	}

	for (auto & c: outConnectors)
	{
		c->setBounds(rr.removeFromTop(connectorSize));
		rr.removeFromTop(10);
	}
}

Rectangle<int> NodeViewUI::getMainBounds()
{
	return BaseItemUI::getMainBounds().reduced(connectorSize, 0);
}

Rectangle<int> NodeViewUI::getFeedbackBounds()
{
	return getMainBounds().withTrimmedTop(headerHeight + headerGap + 10).reduced(8);
}

void NodeViewUI::addConnector(bool isInput, NodeConnectionSlot * slot, bool resizeAfter)
{
	Connector * c = getConnectorForSlot(isInput, slot);
	if (c == nullptr)
	{
		c = new Connector(slot);
		if (isInput) inConnectors.add(c);
		else outConnectors.add(c);
		addAndMakeVisible(c);

	}

	if(resizeAfter) resized();
}

void NodeViewUI::removeConnector(bool isInput, NodeConnectionSlot * slot)
{
	Connector * c = getConnectorForSlot(isInput, slot);
	removeConnector(isInput, c);
}

void NodeViewUI::removeConnector(bool isInput, Connector * c)
{
	if (c == nullptr) return;

	removeChildComponent(c);

	if (isInput) inConnectors.removeObject(c);
	else outConnectors.removeObject(c);

	resized();
}

NodeViewUI::Connector * NodeViewUI::getConnectorForSlot(bool isInput, NodeConnectionSlot * slot)
{
	if (isInput)
	{
		for (auto & s : inConnectors) if (s->slot == slot) return s;
	} else {
		for (auto & s : outConnectors) if (s->slot == slot) return s;
	}

	return nullptr;
}

NodeViewUI::Connector * NodeViewUI::getConnectorWithName(bool isInput, const String & name)
{
	if (isInput)
	{
		for (auto & s : inConnectors) if (s->name == name) return s;
	} else {
		for (auto & s : outConnectors) if (s->name == name) return s;
	}

	return nullptr;
}

void NodeViewUI::newMessage(const Node::NodeEvent & e)
{
	switch (e.type)
	{
	case Node::NodeEvent::INSLOT_ADDED:
	case Node::NodeEvent::OUTSLOT_ADDED:
		addConnector(e.type == Node::NodeEvent::INSLOT_ADDED, e.slot);
		break;

	case Node::NodeEvent::INSLOT_REMOVED:
	case Node::NodeEvent::OUTSLOT_REMOVED:
	{

		bool isInput = e.type == Node::NodeEvent::INSLOT_REMOVED;
		removeConnector(isInput, getConnectorWithName(isInput, e.params.toString()));
	}
	break;
	}
}

NodeViewUI::Connector::Connector(NodeConnectionSlot * slot) :
	name(slot->name),
	slot(slot)
{
	setTooltip(name + " (" + NodeConnectionUI::getStringForConnectionType(slot->type) + ")");
}

NodeViewUI::Connector::~Connector()
{
}

void NodeViewUI::Connector::paint(Graphics & g)
{
	Colour c = NodeConnectionUI::getColorForType(slot->type);
	if (isMouseOver()) c = c.brighter();
	g.setGradientFill(ColourGradient(c, getLocalBounds().getCentre().toFloat(), slot->isConnected()?c.brighter():c.darker(.8f),Point<float>(),true));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
}

void NodeViewUI::Connector::mouseDown(const MouseEvent & e)
{
}

void NodeViewUI::Connector::mouseDrag(const MouseEvent & e)
{
}

void NodeViewUI::Connector::mouseUp(const MouseEvent & e)
{
}
