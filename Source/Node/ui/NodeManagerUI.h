/*
  ==============================================================================

    NodeManagerUI.h
    Created: 14 Apr 2018 2:10:12am
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../NodeManager.h"
#include "NodeViewUI.h"
#include "NodeConnectionManagerUI.h"

class NodeManagerUI :
	public BaseManagerViewUI<NodeManager, Node, NodeViewUI>,
	public Timer
{
public:
	NodeManagerUI(NodeManager * manager);
	~NodeManagerUI();

	NodeViewUI::Connector * startConnector; //for when connecting
	NodeViewUI::Connector * dropConnector;
	const int dropDistance = 20;

	String dragType; 
	Point<float> dragPosition;
	
	ScopedPointer<NodeConnectionManagerUI> connectionsUI;

	
	void paintOverChildren(Graphics &g) override;
	void drawConnectionCreation(Graphics &g);
	void resized() override;


	NodeViewUI::Connector * getDropCandidate();

	NodeViewUI::Connector * getConnectorForSlot(NodeConnectionSlot * s);

	NodeViewUI * createUIForItem(Node * item) override;

	
	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;
	void mouseEnter(const MouseEvent &e) override;

	void itemDragEnter(const SourceDetails &source) override;
	void itemDragExit(const SourceDetails &source) override;
	void itemDragMove(const SourceDetails &source) override;
	void itemDropped(const SourceDetails &source) override;
	void timerCallback() override;
};