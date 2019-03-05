/*
  ==============================================================================

    NodeBlockEditorToolbox.cpp
    Created: 5 Mar 2019 10:21:24am
    Author:  bkupe

  ==============================================================================
*/

#include "NodeBlockEditorToolbox.h"
#include "Node/NodeManager.h"

NodeBlockEditorToolbox::NodeBlockEditorToolbox() :
	Component("Toolbox")
{
	for (auto &d : NodeFactory::getInstance()->defs)
	{
		NodeBlockEditorToolItem * i = new NodeBlockEditorToolItem(d->type);
		addAndMakeVisible(i);
		items.add(i);
	}
}

NodeBlockEditorToolbox::~NodeBlockEditorToolbox()
{
}

void NodeBlockEditorToolbox::paint(Graphics & g)
{
	g.fillAll(BG_COLOR.darker());
}

void NodeBlockEditorToolbox::resized()
{
	Rectangle<int> r = getLocalBounds();
	for (auto &i : items)
	{
		i->setBounds(r.removeFromLeft(60).reduced(2));
	}
}