/*
  ==============================================================================

    PropTargetFilterManager.cpp
    Created: 5 Feb 2019 12:43:58pm
    Author:  jonglissimo

  ==============================================================================
*/

#include "PropTargetFilterManager.h"
#include "PropTargetFilter.h"

PropTargetFilterManager::PropTargetFilterManager() :
	BaseManager<PropTargetFilter>("Filters")
{
	selectItemWhenCreated = false;
	managerFactory = &factory;
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Global ID", PropFilterGlobalID::create));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Family", PropFilterPropFamily::create));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Type", PropFilterPropType::create));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Cluster", PropFilterCluster::create));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Script", PropFilterScript::create));
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
