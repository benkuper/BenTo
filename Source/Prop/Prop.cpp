/*
  ==============================================================================

	Prop.cpp
	Created: 10 Apr 2018 6:59:25pm
	Author:  Ben

  ==============================================================================
*/

#include "Prop.h"

Prop::Prop(var) :
	BaseItem("Prop")
{
	id = addIntParameter("ID", "Prop ID", 0, 0, INT_MAX);
	//id->setControllableFeedbackOnly(true);
	resolution = addIntParameter("Resolution", "Number of controllable colors in the prop", 1, 1, INT_MAX);
}

Prop::~Prop()
{
}
