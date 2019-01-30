/*
  ==============================================================================

    LighttoysVisualProp.cpp
    Created: 30 Jan 2019 10:04:30am
    Author:  bkupe

  ==============================================================================
*/

#include "LighttoysVisualProp.h"

LighttoysVisualProp::LighttoysVisualProp(var params) :
	Prop(getTypeString(), params)
{
}

LighttoysVisualProp::~LighttoysVisualProp()
  {
  }

void LighttoysVisualProp::sendColorsToPropInternal()
{
}
