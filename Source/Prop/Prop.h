/*
  ==============================================================================

    Prop.h
    Created: 10 Apr 2018 6:59:25pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "LightBlock/LightBlock.h"

class Prop :
	public BaseItem
{
public:
	Prop(var params = var());
	~Prop();

	IntParameter * id;
	IntParameter * resolution;

};