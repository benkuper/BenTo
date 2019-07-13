/*
  ==============================================================================

    ModelNode.h
    Created: 13 Apr 2018 11:30:06pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "../../ColorNode.h"

class LightBlock;
class LightBlockColorProvider;

class ModelNode :
	public ColorNode
{
public:
	ModelNode(var params = var());
	~ModelNode();

	std::unique_ptr<LightBlock> currentBlock;
	TargetParameter * activeProvider;

	void setBlockFromProvider(LightBlockColorProvider * provider);

	void buildSlots();

	Array<Colour> getColors(Prop * p, double time, var params) override;

	void onContainerParameterChanged(Parameter * p) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
	
	NodeViewUI * createUI() override;

	String getTypeString() const override { return "Model"; }
	static ModelNode * create(var params) { return new ModelNode(params); }
};