/*
  ==============================================================================

    PropTargetFilterManager.h
    Created: 5 Feb 2019 12:43:58pm
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "PropTargetFilter.h"

class PropTargetFilterManager :
	public BaseManager<PropTargetFilter>
{
public:
	PropTargetFilterManager(PropClusterGroupManager * clusterGroupManager = nullptr);
	~PropTargetFilterManager();

	Factory<PropTargetFilter> factory;

	PropClusterGroupManager* clusterGroupManager;
	
	EnumParameter* operationMode;

	int getTargetIDForProp(Prop *);

	void addItemInternal(PropTargetFilter *, var data) override;
	void removeItemInternal(PropTargetFilter *) override;

	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable *c) override;

	class  FilterManagerListener
	{
	public:
		/** Destructor. */
		virtual ~FilterManagerListener() {}
		virtual void filtersChanged() {}
	};

	ListenerList<FilterManagerListener> filerManagerListeners;
	void addFilterManagerListener(FilterManagerListener * newListener) { filerManagerListeners.add(newListener); }
	void removeFilterManagerListener(FilterManagerListener* listener) { filerManagerListeners.remove(listener); }

};