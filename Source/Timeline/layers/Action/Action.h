/*
  ==============================================================================

    Action.h
    Created: 7 Mar 2019 8:07:09pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class Action :
	public TimeTrigger
{
public:
	Action(StringRef name = "Action");
	~Action();
};