/*
  ==============================================================================

    NodeConnectionUI.h
    Created: 13 Apr 2018 11:26:29pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../NodeConnection.h"
#include "NodeViewUI.h"

class NodeConnectionUI :
	public BaseItemMinimalUI<NodeConnection>,
	public ComponentListener
{
public:
	NodeConnectionUI(NodeConnection * nc);
	~NodeConnectionUI();

	NodeViewUI::Connector * sourceConnector;
	NodeViewUI::Connector * destConnector;

	Path path;
	Path hitPath;

	void paint(Graphics &g) override;
	void resized() override;
	
	bool hitTest(int x, int y) override;

	void buildPath();
	void buildHitPath();

	void setConnectors(NodeViewUI::Connector * sourceConnector, NodeViewUI::Connector * destConnector);
	void updateBounds();

	void componentMovedOrResized(Component & c, bool, bool) override;
	void componentBeingDeleted(Component &c) override;

	static Colour getColorForType(ConnectionType t) {
		switch (t)
		{
		case ConnectionType::ColorBlock:
			return Colours::cornflowerblue;

		case ConnectionType::Number:
			return Colours::limegreen;

		case ConnectionType::Color:
			return Colours::hotpink;
		}

		return Colours::black;
	}

	static String getStringForConnectionType(ConnectionType t) {
		switch (t)
		{
		case ConnectionType::ColorBlock: return "Colors";

		case ConnectionType::Number: return "Number";

		case ConnectionType::Color: return "Single Color";
		}

		return "Unknown";
	}
};