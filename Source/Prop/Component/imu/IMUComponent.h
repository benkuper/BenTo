/*
  ==============================================================================

    IMUComponent.h
    Created: 8 May 2020 3:08:55pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../PropComponent.h"

class IMUPropComponent :
    public PropComponent
{
public:
    IMUPropComponent(Prop* prop, var params);
    ~IMUPropComponent();

    Point3DParameter* orientation;
};