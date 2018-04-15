/*
  ==============================================================================

    NodeViewUI.h
    Created: 13 Apr 2018 11:25:28pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../Node.h"
#include "../NodeConnection.h"

class NodeViewUI :
	public BaseItemUI<Node>,
	public Node::AsyncListener
{
public:
	class Connector;

	NodeViewUI(Node * node);
	~NodeViewUI();

	const int connectorSize = 10;

	Prop * propToPreview;

	OwnedArray<Connector> inConnectors;
	OwnedArray<Connector> outConnectors;

	void resized() override;
	Rectangle<int> getMainBounds() override;

	Rectangle<int> getFeedbackBounds();

	void addConnector(bool isInput, NodeConnectionSlot * slot, bool resizeAfter = true);
	void removeConnector(bool isInput, NodeConnectionSlot * slot);
	void removeConnector(bool isInput, Connector * c);
	Connector * getConnectorForSlot(bool isInput, NodeConnectionSlot * slot);
	Connector * getConnectorWithName(bool isInput, const String &name);




	void newMessage(const Node::NodeEvent &e) override;

	class Connector :
		public Component,
		public SettableTooltipClient
	{
	public:
		Connector(NodeConnectionSlot * slot);
		~Connector();

		String name;
		NodeConnectionSlot * slot;

		void paint(Graphics &g) override;

		void mouseDown(const MouseEvent &e) override;
		void mouseDrag(const MouseEvent &e) override;
		void mouseUp(const MouseEvent &e) override;
	}; 
};