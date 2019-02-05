/*
  ==============================================================================

    LighttoysFamily.cpp
    Created: 5 Feb 2019 11:22:27am
    Author:  jonglissimo

  ==============================================================================
*/

#include "LighttoysFamily.h"
#include "ft/LighttoysFTProp.h"

LighttoysFamily::LighttoysFamily() :
	PropFamily("Lighttoys", 30)
{
}

LighttoysFamily::~LighttoysFamily()
{
}

void LighttoysFamily::onContainerParameterChanged(Parameter * p)
{
	PropFamily::onContainerParameterChanged(p);
}
