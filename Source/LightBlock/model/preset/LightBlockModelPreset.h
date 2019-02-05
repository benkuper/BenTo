/*
  ==============================================================================

    LightBlockModelPreset.h
    Created: 12 Apr 2018 1:10:55pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "../LightBlockColorProvider.h"

class LightBlockModel;

class LightBlockModelPreset :
	public LightBlockColorProvider
{
public:
	LightBlockModelPreset(LightBlockModel * model = nullptr, var params = var());
	virtual ~LightBlockModelPreset();

	LightBlockModel * model;
	WeakReference<Inspectable> modelRef;
	ControllableContainer paramsContainer;

	var previousData; //for keeping overriden value and saved dynamic values

	Array<Colour> getColors(Prop * p, double time, var params) override;

	void rebuildArgsFromModel(bool syncValues = true);
	void childStructureChanged(ControllableContainer * cc) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	Array<WeakReference<Controllable>> getModelParameters() override;
		
};