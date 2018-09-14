/*
  ==============================================================================

    ModelNode.h
    Created: 13 Apr 2018 11:30:06pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "../../ColorNode.h"

class ModelNode :
	public ColorNode
{
public:
	ModelNode(var params = var());
	~ModelNode();

	ScopedPointer<LightBlock> currentBlock;
	TargetParameter * activeProvider;

	void setBlockFromProvider(LightBlockColorProvider * provider);

	Array<Colour> getColors(int id, int resolution, double time, var params) override;

	void onContainerParameterChanged(Parameter * p) override;

	String getTypeString() const override { return "Model"; }
	static ModelNode * create(var params) { return new ModelNode(params); }
};