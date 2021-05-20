/*
  ==============================================================================

    NodeBlockEditorToolbox.cpp
    Created: 5 Mar 2019 10:21:24am
    Author:  bkupe

  ==============================================================================
*/

NodeBlockEditorToolbox::NodeBlockEditorToolbox() :
	Component("Toolbox")
{
	for (auto &d : NodeFactory::getInstance()->defs)
	{
		Factory<Node>::Definition* def = dynamic_cast<Factory<Node>::Definition*>(d);
		String t = def->params.getProperty("nodeType","");
		NodeBlockEditorToolItem * i = new NodeBlockEditorToolItem(t == "param", d->type);
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
		i->setBounds(r.removeFromLeft(i->getWidthForHeight(r.getHeight()-4)).reduced(0,2));
		r.removeFromLeft(2);
	}
}