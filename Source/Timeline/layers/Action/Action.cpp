/*
  ==============================================================================

    Action.cpp
    Created: 7 Mar 2019 8:07:09pm
    Author:  bkupe

  ==============================================================================
*/

#include "Action.h"

Action::Action(float time, float flagYPos, const String & name) :
	TimeTrigger(time, flagYPos, name)
{
}

Action::~Action()
  {
  }
