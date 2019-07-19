/*
  ==============================================================================

    SpatItem.h
    Created: 23 Apr 2018 9:43:20pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Prop/TargetFilter/PropTargetFilterManager.h"

class SpatItem :
	public BaseItem
{
public:
	SpatItem();
	~SpatItem();

	Array<Point<float>> points;
	Array<Colour> colors;

	BoolParameter * isDefault;
	PropTargetFilterManager filterManager;


	EnumParameter * shape;
	IntParameter * resolution;

	Point2DParameter * startPos;
	Point2DParameter * endPos;


	void updatePoints();
	void onContainerParameterChangedInternal(Parameter *) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
};