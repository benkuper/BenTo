/*
  ==============================================================================

    SmarballProp.cpp
    Created: 21 Sep 2018 8:31:28pm
    Author:  Ben

  ==============================================================================
*/

#include "SmarballProp.h"

SmartballProp::SmartballProp(var params) :
	BentoProp(getTypeString(), "Garnav", params)
{
	type->setValueWithData(BALL);
	accelerometer = sensorsCC.addPoint3DParameter("Accelerometer", "Accelerometer data");
	accelerometer->setControllableFeedbackOnly(true);

	resolution->setValue(6);
}

SmartballProp::~SmartballProp()
{
}