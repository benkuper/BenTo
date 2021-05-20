/*
  ==============================================================================

    IRPropComponent.h
    Created: 8 May 2020 4:23:53pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class IRPropComponent :
    public PropComponent
{
public:
    IRPropComponent(Prop * prop, var params);
    ~IRPropComponent();

    FloatParameter* brightness;
};