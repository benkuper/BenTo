/*
  ==============================================================================

	GarnavFamily.cpp
	Created: 5 Feb 2019 11:21:43am
	Author:  jonglissimo

  ==============================================================================
*/

#include "GarnavFamily.h"
#include "SmarballProp.h"

GarnavFamily::GarnavFamily() :
	PropFamily("Garnav", 30)
{
}

GarnavFamily::~GarnavFamily()
{
}

void GarnavFamily::onContainerParameterChanged(Parameter * p)
{
	PropFamily::onContainerParameterChanged(p);
}
