/*
  ==============================================================================

	FXComponent.h
	Created: 19 Mar 2023 11:54:09am
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class FXPropComponent :
	public PropComponent
{
public:
	FXPropComponent(Prop* prop, var params);
	~FXPropComponent();

	FloatParameter* offsetSpeed;
	FloatParameter* isolationSpeed;
	FloatParameter* isolationSmoothing;
	IntParameter* flipDebounce;
	BoolParameter* showCalibration;

	EnumParameter* isolationAxis; //0 = projectedAngle, 1 = yaw, 2 = pitch, 3 = roll

};