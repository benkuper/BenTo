/*
  ==============================================================================

    SpatItem.h
    Created: 23 Apr 2018 9:43:20pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class PropTargetFilterManager;

class SpatItem :
	public BaseItem
{
public:
	SpatItem();
	~SpatItem();

	Array<Point<float>> points;
	Array<Colour> colors;

	BoolParameter * isDefault;
	std::unique_ptr<PropTargetFilterManager> filterManager;


	EnumParameter * shape;
	IntParameter* startIndex;
	IntParameter * resolution;

	ControllableContainer handlesCC;
	Array<Point2DParameter*> handles;

	bool isUpdatingHandles;

	void updateHandles();

	void updateCustomHandles();

	void updatePoints();
	void onContainerParameterChangedInternal(Parameter *) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
};