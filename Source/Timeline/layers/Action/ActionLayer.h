/*
  ==============================================================================

    ActionLayer.h
    Created: 7 Mar 2019 7:32:29pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class ActionManager;

class ActionLayer :
	public TriggerLayer
{
public:
	ActionLayer(Sequence * _sequence, var params = var());
	~ActionLayer();

	String getTypeString() const override { return "Actions"; }
	static ActionLayer * create(Sequence * s, var params) { return new ActionLayer(s, params); }
};