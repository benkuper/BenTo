/*
  ==============================================================================

    PropManager.h
    Created: 10 Apr 2018 6:59:29pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Prop.h"

class PropManager :
	public BaseManager<Prop>
{
public:
	juce_DeclareSingleton(PropManager, true)
	
	PropManager();
	~PropManager();

	Factory<Prop> factory;
	Trigger * autoAssignIdTrigger;

	void onContainerTriggerTriggered(Trigger * t) override;
};