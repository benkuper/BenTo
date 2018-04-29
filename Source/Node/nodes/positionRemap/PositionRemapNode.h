/*
  ==============================================================================

    PositionRemapNode.h
    Created: 13 Apr 2018 11:30:20pm
    Author:  Ben

  ==============================================================================
*/


#pragma once
#include "../../ColorNode.h"

class PositionRemapNode :
	public ColorNode
{
public:
	PositionRemapNode(var params = var());
	~PositionRemapNode();

	enum RemapMode { RESIZE, CROP };

	EnumParameter * mode;
	Parameter * position;
	Parameter * size;

	ColorSlot * inColors;
	ParameterSlot * positionSlot;
	ParameterSlot * sizeSlot;

	Array<Colour> getColors(int id, int resolution, float time, var params) override;

	String getTypeString() const override { return "Position Remap"; }
	static PositionRemapNode * create(var params) { return new PositionRemapNode(params); }
};