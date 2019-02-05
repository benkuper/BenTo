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
	PropTargetFilterManager();
	~PropTargetFilterManager();

	Factory<PropTargetFilter> factory;

	int getTargetIDForProp(Prop *);
};