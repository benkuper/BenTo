/*
  ==============================================================================

    PropCluster.h
    Created: 5 Feb 2019 11:12:24am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "../Prop.h"

class PropCluster :
	public BaseItem,
	public GenericControllableManager::Listener
{
public:
	PropCluster();
	~PropCluster();

	GenericControllableManager propIDManager;
	Array<int,CriticalSection> propIDs;

	void updateIDs();

	void controllableFeedbackUpdate(ControllableContainer * cc, Controllable * c) override;
	void itemAdded(GenericControllableItem *) override;
	void itemRemoved(GenericControllableItem *) override;

	Array<Prop *> getProps();
	bool isPropIsInCluster(Prop *) const;
	int getLocalPropID(Prop *p) const;

};