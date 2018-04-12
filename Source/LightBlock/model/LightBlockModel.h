/*
  ==============================================================================

    LightBlockModel.h
    Created: 10 Apr 2018 7:12:39pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlockModelParameterManager.h"

class LightBlock;

class Prop;

class LightBlockModel :
	public BaseItem
{
public:
	LightBlockModel(const String &name = "LightBlockModel", var params = var());
	~LightBlockModel();

	ScopedPointer<ControllableContainer> paramsContainer;

	virtual Array<WeakReference<Parameter>> getModelParameters();
	virtual Array<Colour> getColors(LightBlock * block, var params = var());

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void onContainerParameterChangedInternal(Parameter * p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
	void childStructureChanged(ControllableContainer * cc) override;
	
	class ModelListener
	{
	public:
		virtual ~ModelListener() {}
		virtual void modelParametersChanged(LightBlockModel *) {}
	};

	ListenerList<ModelListener> modelListeners;
	void addCommandModelListener(ModelListener* newListener) { modelListeners.add(newListener); }
	void removeCommandModelListener(ModelListener* listener) { modelListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlockModel)
};
