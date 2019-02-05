/*
  ==============================================================================

    GarnavFamily.h
    Created: 5 Feb 2019 11:21:59am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "../../PropFamily.h"

class GarnavFamily :
	public PropFamily
{
public:
	GarnavFamily();
	~GarnavFamily();


	void onContainerParameterChanged(Parameter * p) override;
};