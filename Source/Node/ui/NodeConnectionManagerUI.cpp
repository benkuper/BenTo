/*
  ==============================================================================

    NodeConnectionManagerUI.cpp
    Created: 14 Apr 2018 6:17:42pm
    Author:  Ben

  ==============================================================================
*/

NodeConnectionManagerUI::NodeConnectionManagerUI(NodeConnectionManager * manager, NodeManagerUI * nmui) :
	ManagerUI("Connections",manager,false),
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
