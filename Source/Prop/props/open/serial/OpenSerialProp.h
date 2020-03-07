/*
  ==============================================================================

    OpenSerialProp.h
    Created: 7 Mar 2020 4:56:09pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../OpenProp.h"
#include "Common/Serial/SerialDeviceParameter.h"

class OpenSerialProp :
	public OpenProp
{
public:
	OpenSerialProp(var params = var());
	~OpenSerialProp();

	SerialDeviceParameter* deviceParam;

	static var sendBytesFromScript(const var::NativeFunctionArgs& a);

	String getTypeString() const override { return "Open Serial"; }
	static OpenSerialProp* create(var params) { return new OpenSerialProp(params); }
};