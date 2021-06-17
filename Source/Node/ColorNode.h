/*
  ==============================================================================

	ColorNode.h
	Created: 14 Apr 2018 1:00:32pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class Prop;

class ColorNode :
	public Node
{
public:
	ColorNode(const String &name, var params = var());
	virtual ~ColorNode();

	ColorSlot * mainColorsSlot;

	virtual Array<Colour> getColors(Prop * p, double time, var params);
	virtual Array<Colour> getColorsInternal(Prop * p, double time, var params, var localParams);

	virtual NodeViewUI * createUI() override;
};