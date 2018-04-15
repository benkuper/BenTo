/*
  ==============================================================================

    ColorRemapNode.h
    Created: 13 Apr 2018 11:29:57pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../ColorNode.h"

class ColorRemapNode :
	public ColorNode
{
public:
	ColorRemapNode(var params = var());
	~ColorRemapNode();

	String getTypeString() const override { return "Color Remap"; }
	static ColorRemapNode * create(var params) { return new ColorRemapNode(params); }
};