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
	public BaseItem,
	public LightBlock::LightBlockListener
{
public:
	LightBlockClip(LightBlockLayer * layer = nullptr, float time = 0);
	~LightBlockClip();

	LightBlockLayer * layer;
	ScopedPointer<LightBlock> currentBlock;
	
	TargetParameter * activeProvider;

	FloatParameter * startTime;
	FloatParameter * length;

	FloatParameter * clipPosition;
	FloatParameter * clipSize;

	FloatParameter * fadeIn;
	FloatParameter * fadeOut;

	void setBlockFromProvider(LightBlockColorProvider * provider);
	Array<Colour> getColors(Prop * p, double time, var params);
	
	float getTimeForRelativePosition(float t, bool absoluteTime = true);

	bool isInRange(float time);

	void onContainerParameterChanged(Parameter * p) override;
	virtual void blockParamControlModeChanged(Parameter *p) override;

	//void itemAdded(ParameterAutomation * p) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	String getTypeString() const override { return "LightBlockClip"; }
};