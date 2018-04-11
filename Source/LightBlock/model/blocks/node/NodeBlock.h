/*
  ==============================================================================

    NodeBlock.h
    Created: 10 Apr 2018 6:59:08pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../LightBlockModel.h"

class NodeBlock :
	public LightBlockModel
{
public:
	NodeBlock(var params = var());
	~NodeBlock();

	String getTypeString() const override { return "Node"; }

	static NodeBlock * create(var params) { return new NodeBlock(params); }
};