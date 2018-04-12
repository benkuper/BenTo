/*
  ==============================================================================

    PropManagerUI.cpp
    Created: 10 Apr 2018 7:00:09pm
    Author:  Ben

  ==============================================================================
*/

#include "PropManagerUI.h"

PropManagerUI::PropManagerUI(const String &name, PropManager * m) :
	BaseManagerShapeShifterUI(name, m)
{
	setDefaultLayout(HORIZONTAL);
}

PropManagerUI::~PropManagerUI()
{
}
