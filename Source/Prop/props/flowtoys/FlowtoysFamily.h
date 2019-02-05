/*
  ==============================================================================

    FlowtoysFamily.h
    Created: 5 Feb 2019 11:21:43am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "../../PropFamily.h"

class FlowtoysFamily :
	public PropFamily
{
public:
	FlowtoysFamily();
	~FlowtoysFamily();

	IntParameter * targetPropFPS;

	void onContainerParameterChanged(Parameter * p) override;
};