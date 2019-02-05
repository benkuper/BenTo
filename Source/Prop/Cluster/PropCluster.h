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
	public BaseItem
{
public:
	PropCluster();
	~PropCluster();

	//ControllableContainer propIDsCC;
	Array<int,CriticalSection> propIDs;

	void updateIDs();

	void onContainerParameterChanged(Parameter *p) override;
	void onControllableAdded(Controllable *) override;
	void onControllableRemoved(Controllable *) override;

	Array<Prop *> getProps();
	bool isPropIsInCluster(Prop *) const;
	int getLocalPropID(Prop *p) const;

	void loadJSONData(var data, bool createIfNotThere = false) override;

};