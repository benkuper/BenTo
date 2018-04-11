/*
  ==============================================================================

    FlowPoiProp.h
    Created: 10 Apr 2018 10:31:01pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../../Prop.h"

class FlowPoiProp :
	public Prop
{
public:
	FlowPoiProp(var params);
	~FlowPoiProp();

	String getTypeString() const override { return "FlowPoi"; }

	static FlowPoiProp * create(var params) { return new FlowPoiProp(params); }
};