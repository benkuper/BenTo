/*
  ==============================================================================

    FlowClubProp.h
    Created: 10 Apr 2018 10:29:13pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../../Prop.h"

class FlowClubProp :
	public Prop
{
public:
	FlowClubProp(var params);
	~FlowClubProp();

	String getTypeString() const override { return "FlowClub"; }

	static FlowClubProp * create(var params) { return new FlowClubProp(params); }
};