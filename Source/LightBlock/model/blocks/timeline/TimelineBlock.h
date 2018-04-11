/*
  ==============================================================================

    TimelineBlock.h
    Created: 10 Apr 2018 6:59:02pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../LightBlockModel.h"

class TimelineBlock :
	public LightBlockModel
{
public:
	TimelineBlock(var params = var());
	~TimelineBlock();

	String getTypeString() const override { return "Timeline"; }

	static TimelineBlock * create(var params) { return new TimelineBlock(params); }
};