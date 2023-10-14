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
	var localParams = getLocalParams(params);
	return getColorsInternal(p, time, params, localParams);
}

Array<Colour> ColorNode::getColorsInternal(Prop * p, double time, var params, var localParams)
{
	Array<Colour> result;
	result.resize(p->getResolution());
	result.fill(Colours::transparentBlack);
	return result;
}

NodeViewUI * ColorNode::createUI()
{
	return new ColorNodeViewUI(this);
}
