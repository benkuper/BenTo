/*
  ==============================================================================

    LightBlockModel.h
    Created: 10 Apr 2018 7:12:39pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlockModelParameterManager.h"

class Prop;

class LightBlockModel :
	public BaseItem,
	public LightBlockModelParameterManager::Listener
{
public:
	LightBlockModel(const String &name = "LightBlockModel", var params = var());
	~LightBlockModel();

	LightBlockModelParameterManager parameters;

	virtual Array<Colour> getColours(Prop * prop, var params);

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void onContainerParameterChangedInternal(Parameter * p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
	void itemAdded(LightBlockModelParameter *) override;
	void itemRemoved(LightBlockModelParameter *) override;
	void itemsReordered() override;

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
