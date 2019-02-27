/*
  ==============================================================================

    LightBlockClip.h
    Created: 17 Apr 2018 5:10:24pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Prop/Prop.h"

class LightBlockLayer;

class LightBlockClip :
	public LayerBlock,
	public LightBlock::LightBlockListener
{
public:
	LightBlockClip(LightBlockLayer * layer = nullptr, float time = 0);
	~LightBlockClip();

	LightBlockLayer * layer;
	ScopedPointer<LightBlock> currentBlock;
	
	TargetParameter * activeProvider;

	FloatParameter * fadeIn;
	FloatParameter * fadeOut;

	void setBlockFromProvider(LightBlockColorProvider * provider);
	Array<Colour> getColors(Prop * p, double absoluteTime, var params);
	
	virtual void blockParamControlModeChanged(Parameter *p) override;

	//void itemAdded(ParameterAutomation * p) override;

	void setCoreLength(float value, bool stretch = false, bool stickToCoreEnd = false) override;

	void onContainerParameterChangedInternal(Parameter * p) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	String getTypeString() const override { return "LightBlockClip"; }
};