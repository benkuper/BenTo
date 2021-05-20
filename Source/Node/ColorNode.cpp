/*
  ==============================================================================

	ColorNode.cpp
	Created: 14 Apr 2018 1:00:32pm
	Author:  Ben

  ==============================================================================
*/

ColorNode::ColorNode(const String & name, var params) : 
	Node(name, params)
{
	mainColorsSlot = addColorSlot(false, "Colors");
}

ColorNode::~ColorNode()
{
}

Array<Colour> ColorNode::getColors(Prop * p, double time, var params)
{
	fillWithLocalParams(params);
	return getColorsInternal(p, time, params);
}

Array<Colour> ColorNode::getColorsInternal(Prop * p, double time, var params)
{
	Array<Colour> result;
	result.resize(p->resolution->intValue());
	result.fill(Colours::transparentBlack);
	return result;
}

NodeViewUI * ColorNode::createUI()
{
	return new ColorNodeViewUI(this);
}
