/*
  ==============================================================================

    LightBlockModel.h
    Created: 10 Apr 2018 7:12:39pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlockModelParameterManager.h"
#include "preset/LightBlockModelPresetManager.h"
#include "LightBlockColorProvider.h"

class LightBlock;

class Prop;

class LightBlockModel :
	public LightBlockColorProvider
{
public:
	LightBlockModel(const String &name = "LightBlockModel", var params = var());
	~LightBlockModel();

	void clear() override;

	ScopedPointer<ControllableContainer> paramsContainer;
	LightBlockModelPresetManager presetManager;

	virtual Array<WeakReference<Parameter>> getModelParameters() override;

	virtual void updateColorsForBlock(LightBlock * block, var params = var()) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
	void childStructureChanged(ControllableContainer * cc) override;


	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlockModel)
};
