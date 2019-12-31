/*
  ==============================================================================

    LightrixHoop.h
    Created: 12 Dec 2019 10:03:27am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../bento/BentoProp.h"

class LightrixHoopProp :
	public BentoProp
{
public:
	LightrixHoopProp(var params = var());
	~LightrixHoopProp();

	String getTypeString() const override { return "Lightrix Hoop"; }
	static LightrixHoopProp* create(var params) { return new LightrixHoopProp(params); }

};