/*
  ==============================================================================

    IMUComponent.cpp
    Created: 8 May 2020 3:08:55pm
    Author:  bkupe

  ==============================================================================
*/

#include "IMUComponent.h"

IMUPropComponent::IMUPropComponent(Prop* prop, var params) :
    PropComponent(prop, "IMU", true)
{
    enabled->setValue(false);
    
    orientation = addPoint3DParameter("Orientation", "Orientation of the prop");
    orientation->setControllableFeedbackOnly(true);
    orientation->setBounds(-180, -90, -180, 180, 90, 180);
}

IMUPropComponent::~IMUPropComponent()
{
}
