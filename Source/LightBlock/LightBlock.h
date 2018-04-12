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
	public LightBlockModel::ModelListener,
	public Timer
{
public:
	LightBlock(LightBlockModel * model, Prop * prop, var params = var());
	virtual ~LightBlock();

	Prop * prop; 
	LightBlockModel * model;
	WeakReference<Inspectable> modelRef;
	
	ControllableContainer paramsContainer;

	Array<Colour> getColors();

	void timerCallback();

	void rebuildArgsFromModel();
	void modelParametersChanged(LightBlockModel *) override;
	 
	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	static LightBlock * create(LightBlockModel * model, Prop * prop, var params) { return new LightBlock(model, prop, params); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlock)
};