/*
  ==============================================================================

    PatternBlock.h
    Created: 10 Apr 2018 6:58:57pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../LightBlockModel.h"

class PatternBlock :
	public LightBlockModel
{
public:
	PatternBlock(const String &name = "Pattern", var params = var());
	~PatternBlock();

	String getTypeString() const override { return "Pattern"; }
};