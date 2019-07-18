/*
  ==============================================================================

    CompositeNode.h
    Created: 13 Apr 2018 11:30:02pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "../../ColorNode.h"

class CompositeNode :
	public ColorNode
{
public:
	CompositeNode(var params = var());
	~CompositeNode();

	enum BlendMode { ADD, ALPHA, SUBTRACT, MIX, MAX, MIN};

	ColorSlot * c1;
	ColorSlot * c2;

	EnumParameter * blendMode;
	FloatParameter* mix;

	Array<Colour> getColorsInternal(Prop * p, double time, var params) override;

	void onContainerParameterChangedInternal(Parameter* p) override;

	String getTypeString() const override { return "Composite"; }
	static CompositeNode * create(var params) { return new CompositeNode(params); }
};