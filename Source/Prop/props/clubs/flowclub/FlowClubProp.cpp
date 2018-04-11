/*
  ==============================================================================

    FlowClubProp.cpp
    Created: 10 Apr 2018 10:29:13pm
    Author:  Ben

  ==============================================================================
*/

#include "FlowClubProp.h"

FlowClubProp::FlowClubProp(var params) :
	Prop(params)
{
	resolution->setValue(32);
}

FlowClubProp::~FlowClubProp()
{
}
