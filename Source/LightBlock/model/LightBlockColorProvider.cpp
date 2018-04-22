/*
  ==============================================================================

	LightBlockColorProvider.cpp
	Created: 12 Apr 2018 2:10:56pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockColorProvider.h"
#include "Prop/PropManager.h"

LightBlockColorProvider::LightBlockColorProvider(const String & name, bool canBeDisabled, bool canHaveScripts) :
	BaseItem(name, canBeDisabled, canHaveScripts)
{
	assignToAll = addTrigger("Assign To All", "Assign this Model or Preset to all props");
	viewUISize->isSavable = false;
}

LightBlockColorProvider::~LightBlockColorProvider() {
	LightBlockColorProvider::masterReference.clear();
}


void LightBlockColorProvider::onContainerTriggerTriggered(Trigger * t)
{
	BaseItem::onContainerTriggerTriggered(t);
	if (t == assignToAll)
	{
		for (auto & p : PropManager::getInstance()->items)
		{
			p->activeProvider->setValueFromTarget(this);
		}
	}

}
