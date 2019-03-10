/*
  ==============================================================================

    ActionManager.h
    Created: 7 Mar 2019 7:55:04pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "Action.h"

class ActionLayer;

class ActionManager :
	public TimeTriggerManager
{
public:
	ActionManager(ActionLayer * layer, Sequence * sequence);
	~ActionManager();

	
	TimeTrigger * createItem() override;
     void addTriggerAt(float time = 0, float flagY = 0) override;

};