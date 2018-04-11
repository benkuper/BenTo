/*
  ==============================================================================

    ScriptBlock.h
    Created: 10 Apr 2018 6:59:13pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../LightBlockModel.h"

class ScriptBlock :
	public LightBlockModel
{
public:
	ScriptBlock(var params = var());
	~ScriptBlock();

	String getTypeString() const override { return "Script"; }

	static ScriptBlock * create(var params) { return new ScriptBlock(params); }
};