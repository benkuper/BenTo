/*
  ==============================================================================

    FlowClubProp.h
    Created: 10 Apr 2018 10:29:13pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../FlowtoysProp.h"

class FlowClubProp :
	public FlowtoysProp
{
public:
	FlowClubProp(var params);
	~FlowClubProp();

	static const int vidFilter = 0x00;
	static const int pidFilter = 0x00;

	virtual void clearItem() override;

	BoolParameter * button;

	ControllableContainer irCC;
	FloatParameter * irLevel;

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *c) override;


	String getTypeString() const override { return  FlowClubProp::getTypeStringStatic(); }
	static const String getTypeStringStatic() { return "Flowtoys Creator Club"; }

	static FlowClubProp * create(var params) { return new FlowClubProp(params); } 

};