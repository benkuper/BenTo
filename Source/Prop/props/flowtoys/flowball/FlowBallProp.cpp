/*
  ==============================================================================

    FlowBallProp.cpp
    Created: 10 Apr 2018 10:30:28pm
    Author:  Ben

  ==============================================================================
*/

#include "FlowBallProp.h"

FlowBallProp::FlowBallProp(var params) :
	FlowtoysProp(getTypeString(),params)
{
	resolution->setValue(1); 
}

FlowBallProp::~FlowBallProp()
{
}
