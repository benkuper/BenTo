/*
  ==============================================================================

	PropNode.h
	Created: 13 Apr 2018 11:29:28pm
	Author:  Ben

  ==============================================================================
*/

#pragma once
#include "../../ColorNode.h"

class PropNode :
	public ColorNode
{
public:
	PropNode(var params = var());
	~PropNode();

	ColorSlot * inColors;

	Array<Colour> getColorsInternal(Prop * p, double time, var params) override;

	String getTypeString() const override { return "Prop"; }
	static PropNode * create(var params) { return new PropNode(params); }
};