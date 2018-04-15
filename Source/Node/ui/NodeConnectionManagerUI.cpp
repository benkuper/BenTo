/*
  ==============================================================================

    NodeConnectionManagerUI.cpp
    Created: 14 Apr 2018 6:17:42pm
    Author:  Ben

  ==============================================================================
*/

#include "NodeConnectionManagerUI.h"
#include "NodeManagerUI.h"

NodeConnectionManagerUI::NodeConnectionManagerUI(NodeConnectionManager * manager, NodeManagerUI * nmui) :
	BaseManagerUI("Connections",manager,false),
	nmui(nmui)
{
	bringToFrontOnSelect = false;
	addExistingItems();
}

NodeConnectionManagerUI::~NodeConnectionManagerUI()
{
}

void NodeConnectionManagerUI::resized()
{
	for (auto &i : itemsUI) i->updateBounds();
}

void NodeConnectionManagerUI::paint(Graphics & g)
{
}

void NodeConnectionManagerUI::addItemUIInternal(NodeConnectionUI * nui)
{
	NodeViewUI::Connector * sourceC = nmui->getConnectorForSlot(nui->item->sourceSlot);
	NodeViewUI::Connector * destC = nmui->getConnectorForSlot(nui->item->destSlot);
	if(sourceC != nullptr && destC != nullptr) nui->setConnectors(sourceC, destC);
}
