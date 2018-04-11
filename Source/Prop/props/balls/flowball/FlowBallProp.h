/*
  ==============================================================================

    FlowBallProp.h
    Created: 10 Apr 2018 10:30:28pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../../Prop.h"

class FlowBallProp :
	public Prop
{
public:
	FlowBallProp(var params);
	~FlowBallProp();

	String getTypeString() const override { return "FlowBall"; }

	static FlowBallProp * create(var params) { return new FlowBallProp(params); }
};