/*
  ==============================================================================

	FXComponent.cpp
	Created: 19 Mar 2023 11:54:09am
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"

FXPropComponent::FXPropComponent(Prop* prop, var params) :
	PropComponent(prop, "FX")
{
	offsetSpeed = addFloatParameter("Offset Speed", "offsetSpeed", 0, -5, 5);
	isolationSpeed = addFloatParameter("Isolation Speed", "isolationSpeed", 0, -5, 5);
	isolationSmoothing = addFloatParameter("Isolation Smoothing", "isolationSmoothing", 0, 0, 1);

	isolationAxis = addEnumParameter("Isolation Axis", "Axis for isolation");
	isolationAxis->addOption("Projected Angle", 0)->addOption("Yaw", 1)->addOption("Pitch", 2)->addOption("Roll", 3);

	sendValuesOnPropConnected.addArray(Array<Parameter*>{ offsetSpeed, isolationSpeed, isolationSmoothing, isolationAxis});
}

FXPropComponent::~FXPropComponent()
{
}
