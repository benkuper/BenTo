/*
  ==============================================================================

    PropCluster.cpp
    Created: 5 Feb 2019 11:12:24am
    Author:  jonglissimo

  ==============================================================================
*/

#include "PropCluster.h"
#include "../PropManager.h"

PropCluster::PropCluster() :
	BaseItem("Cluster",false,false),
	propIDManager("Prop IDs",false,false,false)
{
	propIDManager.userAddControllablesFilters.add(IntParameter::getTypeStringStatic());
}

PropCluster::~PropCluster()
{
}

void PropCluster::updateIDs()
{
	propIDs.getLock().enter();
	propIDs.clear();
	for (auto &p : propIDManager.items)
	{
		if (p->controllable->type != Controllable::INT) continue;
		propIDs.add(((IntParameter *)p)->intValue());
	}
	propIDs.getLock().exit();
}

void PropCluster::controllableFeedbackUpdate(ControllableContainer * cc, Controllable *)
{
	if (cc == &propIDManager) updateIDs();
}

void PropCluster::itemAdded(GenericControllableItem *)
{
	updateIDs();
}

void PropCluster::itemRemoved(GenericControllableItem *)
{
	updateIDs();
}

Array<Prop*> PropCluster::getProps()
{
	Array<Prop*> result;

	propIDs.getLock().enter();
	for (auto &p : PropManager::getInstance()->items)
	{
		if (isPropIsInCluster(p)); result.add(p);
	}
	propIDs.getLock().exit();

	return result;
}

bool PropCluster::isPropIsInCluster(Prop * p) const
{
	return getLocalPropID(p) >= 0;
}

int PropCluster::getLocalPropID(Prop * p) const
{
	propIDs.getLock().enter();
	int result = propIDs.indexOf(p->globalID->intValue());
	propIDs.getLock().exit();
	return result;
}
