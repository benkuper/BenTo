/*
  ==============================================================================

    PropTargetFilterManager.cpp
    Created: 5 Feb 2019 12:43:58pm
    Author:  jonglissimo

  ==============================================================================
*/

#include "PropTargetFilterManager.h"
#include "PropTargetFilter.h"
#include "Prop/Cluster/PropClusterGroupManager.h"

PropTargetFilterManager::PropTargetFilterManager(PropClusterGroupManager * clusterGroupManager) :
	BaseManager<PropTargetFilter>("Prop Filters"),
	clusterGroupManager(clusterGroupManager)
{
	selectItemWhenCreated = false;
	managerFactory = &factory;
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Global ID", PropFilterGlobalID::create));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Family", PropFilterPropFamily::create));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Type", PropFilterPropType::create));
	if(clusterGroupManager != nullptr) factory.defs.add(new PropFilterCluster::PropFilterClusterDefinition("", "Cluster", PropFilterCluster::create, clusterGroupManager));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Script", PropFilterScript::create));
}

PropTargetFilterManager::~PropTargetFilterManager()
{

}

int PropTargetFilterManager::getTargetIDForProp(Prop * p)
{
	int numProcessed = 0;
	for (auto &f : items)
	{
		if (!f->enabled->boolValue()) continue;
		int result = f->getTargetIDForProp(p);
		if (result >= 0) return result;
		numProcessed++;
	}

	if (numProcessed == 0) return p->globalID->intValue();
	return -1;
}

void PropTargetFilterManager::addItemInternal(PropTargetFilter *, var)
{
	filerManagerListeners.call(&FilterManagerListener::filtersChanged);
}

void PropTargetFilterManager::removeItemInternal(PropTargetFilter *)
{
	filerManagerListeners.call(&FilterManagerListener::filtersChanged);
}

void PropTargetFilterManager::onControllableFeedbackUpdate(ControllableContainer *, Controllable * c)
{
	filerManagerListeners.call(&FilterManagerListener::filtersChanged);
}
