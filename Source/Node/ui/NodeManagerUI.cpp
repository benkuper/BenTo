/*
  ==============================================================================

	NodeManagerUI.cpp
	Created: 14 Apr 2018 2:10:12am
	Author:  Ben

  ==============================================================================
*/

#include "NodeManagerUI.h"


NodeManagerUI::NodeManagerUI(NodeManager * manager) :
	BaseManagerViewUI("Nodes", manager),
	startConnector(nullptr),
	dropConnector(nullptr)
{

	removeMouseListener(this);
	addMouseListener(this, true);

	addExistingItems(false);
	
	connectionsUI = new NodeConnectionManagerUI(&manager->connectionManager, this);
	addAndMakeVisible(connectionsUI);
	connectionsUI->toBack();

	startTimerHz(30);

	resized();
}

NodeManagerUI::~NodeManagerUI()
{
}

void NodeManagerUI::paintOverChildren(Graphics & g)
{
	BaseManagerUI::paintOverChildren(g);
	if (startConnector != nullptr) drawConnectionCreation(g);
}

void NodeManagerUI::drawConnectionCreation(Graphics &g)
{
	Point<float> sp = getLocalPoint(startConnector, startConnector->getLocalBounds().getCentre().toFloat());
	Point<float> dp = dropConnector == nullptr ? getMouseXYRelative().toFloat() : getLocalPoint(dropConnector, dropConnector->getLocalBounds().getCentre().toFloat());
	g.setColour(dropConnector == nullptr ? NodeConnectionUI::getColorForType(startConnector->slot->type) : Colours::yellow);

	Point<float> p1 = startConnector->slot->isInput ? dp : sp;
	Point<float> p2 = startConnector->slot->isInput ? sp : dp;
	Path p;
	p.startNewSubPath(p1);
	p.cubicTo(p1.translated(60, 0), p2.translated(-60, 0), p2);
	g.strokePath(p, PathStrokeType(3));
}

void NodeManagerUI::resized()
{
	if (connectionsUI == nullptr) return;
	connectionsUI->setBounds(getLocalBounds());
	BaseManagerViewUI::resized();
}


NodeViewUI::Connector * NodeManagerUI::getDropCandidate()
{
	for (auto & nui : itemsUI)
	{
		if (nui->item == startConnector->slot->node) continue;
		
		OwnedArray<NodeViewUI::Connector> * arr = startConnector->slot->isInput ? &nui->outConnectors : &nui->inConnectors;
		for (auto &con : *arr)
		{
			if (con->slot->type != startConnector->slot->type) continue;
			if (con->getMouseXYRelative().getDistanceFromOrigin() < dropDistance) return con;
		}
	}

	return nullptr;

}

NodeViewUI::Connector * NodeManagerUI::getConnectorForSlot(NodeConnectionSlot * s)
{
	if (s == nullptr) return nullptr;

	NodeViewUI * nui = getUIForItem(s->node);
	
	if (nui == nullptr) return nullptr;

	OwnedArray<NodeViewUI::Connector> * arr = s->isInput ? &nui->inConnectors : &nui->outConnectors;
	for (auto &con : *arr)
	{
		if (con->slot == s) return con;
	}

	return nullptr;
}

NodeViewUI * NodeManagerUI::createUIForItem(Node * item)
{
	NodeViewUI * nui = item->createUI();
	nui->propToPreview = &manager->prop;
	return nui;
}

void NodeManagerUI::mouseDown(const MouseEvent & e)
{
	BaseManagerViewUI::mouseDown(e);

	NodeViewUI::Connector * c = dynamic_cast<NodeViewUI::Connector *>(e.originalComponent);
	if (c != nullptr)
	{
		startConnector = c;
		setRepaintsOnMouseActivity(true);
	}
}

void NodeManagerUI::mouseDrag(const MouseEvent & e)
{
	BaseManagerViewUI::mouseDrag(e);
	
	//Check for drop candidates
	if (startConnector != nullptr)
	{
		dropConnector = getDropCandidate();
		repaint();
	}
}

void NodeManagerUI::mouseUp(const MouseEvent & e)
{
	BaseManagerViewUI::mouseUp(e);

	if (startConnector != nullptr && dropConnector != nullptr)
	{
		NodeConnectionSlot * source = startConnector->slot->isInput ? dropConnector->slot : startConnector->slot;
		NodeConnectionSlot * dest = startConnector->slot->isInput ? startConnector->slot : dropConnector->slot;
		manager->connectionManager.addConnection(source, dest);
	}

	startConnector = nullptr;
	dropConnector = nullptr;
	repaint();

	setRepaintsOnMouseActivity(false);
}

void NodeManagerUI::mouseEnter(const MouseEvent & e)
{
	BaseManagerViewUI::mouseEnter(e);
	NodeViewUI::Connector * c = dynamic_cast<NodeViewUI::Connector *>(e.originalComponent);
	if (c != nullptr)
	{
		DBG("Mouse Enter on connector : " << c->slot->node->niceName << ":" << c->slot->name);
	}
}

void NodeManagerUI::timerCallback()
{
	for (auto &nui : itemsUI) nui->repaint();
}
