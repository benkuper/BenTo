/*
  ==============================================================================

    LighttoysFamily.h
    Created: 5 Feb 2019 11:22:27am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "../../PropFamily.h"

class LighttoysFamily :
	public PropFamily
{
public:
	LighttoysFamily();
	~LighttoysFamily();

	void onContainerParameterChanged(Parameter * p) override;
};