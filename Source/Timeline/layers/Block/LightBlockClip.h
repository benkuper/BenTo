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
	Prop fakeProp;

	TargetParameter * activeProvider;

	FloatParameter * time;
	FloatParameter * length;

	FloatParameter * clipPosition;
	FloatParameter * clipSize;

	void setBlockFromProvider(LightBlockColorProvider * provider);
	Array<Colour> getColorsForProp(Prop * p);

	bool isInRange(float time);


	void onContainerParameterChanged(Parameter * p) override;

	String getTypeString() const override { return "LightBlockClip"; }
};