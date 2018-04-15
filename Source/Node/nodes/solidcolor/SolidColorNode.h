/*
  ==============================================================================

    SolidColorNode.h
    Created: 13 Apr 2018 11:30:41pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "../../ColorNode.h"

class SolidColorNode :
	public ColorNode
{
public:
	SolidColorNode(var params = var());
	~SolidColorNode();

	ColorParameter * color;

	Array<Colour> getColorsForProp(Prop * p) override;

	String getTypeString() const override { return "Solid Color"; }
	static SolidColorNode * create(var params) { return new SolidColorNode(params); }
};