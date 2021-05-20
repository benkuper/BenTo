/*
  ==============================================================================

    NodeConnectionUI.h
    Created: 13 Apr 2018 11:26:29pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

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

	bool isDraggingModel;
	
	void paint(Graphics &g) override;
	void resized() override;
	
	bool hitTest(int x, int y) override;

	void buildPath();
	void buildHitPath();

	void setConnectors(NodeViewUI::Connector * sourceConnector, NodeViewUI::Connector * destConnector);
	void updateBounds();

	void componentMovedOrResized(Component & c, bool, bool) override;
	void componentBeingDeleted(Component &c) override;
	
	void itemDropped(const SourceDetails &source) override;

	static Colour getColorForType(ConnectionType t);
	static String getStringForConnectionType(ConnectionType t);
};