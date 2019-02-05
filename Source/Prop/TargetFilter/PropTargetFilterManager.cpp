/*
  ==============================================================================

    PropTargetFilterManager.cpp
    Created: 5 Feb 2019 12:43:58pm
    Author:  jonglissimo

  ==============================================================================
*/

#include "PropTargetFilterManager.h"

PropTargetFilterManager::PropTargetFilterManager() :
	BaseManager<PropTargetFilter>("Filters")
{
	selectItemWhenCreated = false;
}

PropTargetFilterManager::~PropTargetFilterManager()
{

}

int PropTargetFilterManager::getTargetIDForProp(Prop * p)
{
	if (items.size() == 0) return p->globalID->intValue();
	for (auto &f : items)
	{
		int result = f->getTargetIDForProp(p);
		if (result >= 0) return result;
	}

	return -1;
}
