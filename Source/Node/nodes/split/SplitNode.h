/*
  ==============================================================================

	SplitNode.h
	Created: 20 Oct 2023 11:40:50am
	Author:  bkupe

  ==============================================================================
*/

#pragma once
class SplitNode :
	public ColorNode
{
public:
	SplitNode(var params = var());
	~SplitNode();

	enum RemapMode { RESIZE, CROP };

	Parameter* positions[3];
	//Parameter* fade;

	ColorSlot* inColors[4];
	ParameterSlot* positionSlots[3];
	//ParameterSlot* fadeSlot;

	Array<Colour> getColorsInternal(Prop* p, double time, var params, var localParams) override;

	DECLARE_TYPE("Split");
	static SplitNode* create(var params) { return new SplitNode(params); }
};