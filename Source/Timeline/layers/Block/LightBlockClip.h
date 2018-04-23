/*
  ==============================================================================

    LightBlockClip.h
    Created: 17 Apr 2018 5:10:24pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Prop/Prop.h"

class LightBlockClip :
	public BaseItem
{
public:
	LightBlockClip(float time = 0);
	~LightBlockClip();

	ScopedPointer<LightBlock> currentBlock;
	
	TargetParameter * activeProvider;

	FloatParameter * startTime;
	FloatParameter * length;

	FloatParameter * clipPosition;
	FloatParameter * clipSize;

	void setBlockFromProvider(LightBlockColorProvider * provider);
	Array<Colour> getColors(int id, int resolution, float time, var params);

	float getTimeForRelativePosition(float t, bool absoluteTime = true);

	bool isInRange(float time);


	void onContainerParameterChanged(Parameter * p) override;

	

	String getTypeString() const override { return "LightBlockClip"; }
};