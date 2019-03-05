/*
  ==============================================================================

    PropClusterGroupManager.h
    Created: 5 Feb 2019 11:53:33am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "PropClusterGroup.h"

class PropClusterGroupManager :
	public BaseManager<PropClusterGroup>
{
public:
	juce_DeclareSingleton(PropClusterGroupManager, true);

	PropClusterGroupManager();
	~PropClusterGroupManager();

	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable * c) override;
};