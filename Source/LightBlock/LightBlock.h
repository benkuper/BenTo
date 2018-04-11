/*
  ==============================================================================

	LightBlock.h
	Created: 10 Apr 2018 6:56:47pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "model/LightBlockModel.h"

class LightBlock :
	public BaseItem,
	public LightBlockModel::ModelListener
{
public:
	LightBlock(LightBlockModel * model, var params);
	virtual ~LightBlock();

	LightBlockModel * model;
	WeakReference<Inspectable> modelRef;

	ControllableContainer paramsContainer;

	void rebuildArgsFromModel();

	void modelParametersChanged(LightBlockModel *) override;

	static LightBlock * create(LightBlockModel * model,  var params) { return new LightBlock(model, params); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlock)
};