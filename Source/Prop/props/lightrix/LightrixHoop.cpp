/*
  ==============================================================================

    LightrixHoop.cpp
    Created: 12 Dec 2019 10:03:27am
    Author:  bkupe

  ==============================================================================
*/

#include "LightrixHoop.h"

LightrixHoopProp::LightrixHoopProp(var params) :
	BentoProp(getTypeString(), "Lightrix", params)
{
	type->setValueWithData(HOOP);
	resolution->setValue(250);
}

LightrixHoopProp::~LightrixHoopProp()
{
}