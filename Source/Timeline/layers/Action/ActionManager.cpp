/*
  ==============================================================================

    ActionManager.cpp
    Created: 7 Mar 2019 7:55:04pm
    Author:  bkupe

  ==============================================================================
*/

#include "ActionManager.h"
#include "ActionLayer.h"

ActionManager::ActionManager(ActionLayer * layer, Sequence * sequence) :
	TimeTriggerManager(layer, sequence)
{
}

ActionManager::~ActionManager()
{
}

TimeTrigger * ActionManager::createItem()
  {
	  return new Action();
  }