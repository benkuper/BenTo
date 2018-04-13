/*
  ==============================================================================

    LightBlockModelGroup.h
    Created: 13 Apr 2018 2:03:02am
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class LightBlockModelGroup :
	public ControllableContainer
{
public:
	LightBlockModelGroup(const String &name) : ControllableContainer(name) {}
	~LightBlockModelGroup() {}

	var getJSONData();
	void loadJSONDataInternal(var data) override;
};