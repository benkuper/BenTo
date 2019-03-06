/*
  ==============================================================================

    SmarballProp.cpp
    Created: 21 Sep 2018 8:31:28pm
    Author:  Ben

  ==============================================================================
*/

#include "SmarballProp.h"

SmartballProp::SmartballProp(const String & name, var params) :
	BentoProp(name, "Garnav", params)
{
	accelerometer = sensorsCC.addPoint3DParameter("Accelerometer", "Accelerometer data");
	accelerometer->setControllableFeedbackOnly(true);
}

SmartballProp::~SmartballProp()
{
}