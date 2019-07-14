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
	BaseItem("Cluster", false, false)
{
	showInspectorOnSelect = false;

	userCanAddControllables = true;
	userAddControllablesFilters.add(IntParameter::getTypeStringStatic());
}

PropCluster::~PropCluster()
{
}

void PropCluster::updateIDs()
{
	propIDs.getLock().enter();
	propIDs.clear();
	Array<WeakReference<Parameter>> params = getAllParameters();
	int index = 0;
	for (auto &p : params)
	{
		p->setNiceName("Prop ID " + String(index));
		if (p->type != Controllable::INT) continue;
		propIDs.add(p->intValue());
		index++;
	}
	propIDs.getLock().exit();
}

void PropCluster::onContainerParameterChanged(Parameter *)
{
	updateIDs();
}

void PropCluster::onControllableAdded(Controllable * c)
{
	c->isRemovableByUser = true;
	c->saveValueOnly = false;
	updateIDs();
}

void PropCluster::onControllableRemoved(Controllable *)
{
	updateIDs();
}


Array<Prop*> PropCluster::getProps()
{
	Array<Prop*> result;

	propIDs.getLock().enter();
	for (auto &p : PropManager::getInstance()->items)
	{
		if (isPropIsInCluster(p)) result.add(p);
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

void PropCluster::loadJSONData(var data, bool createIfNotThere)
{
	ControllableContainer::loadJSONData(data, true); //force creating
}
