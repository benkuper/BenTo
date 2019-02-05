/*
  ==============================================================================

    PropClusterGroup.h
    Created: 5 Feb 2019 11:12:42am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "PropCluster.h"

class PropClusterGroup :
	public BaseItem
{
public:
	PropClusterGroup();
	~PropClusterGroup();

	BoolParameter * sendFeedback;

	BaseManager<PropCluster> clusterManager;

	PropCluster * getClusterForProp(Prop * p, int &localID);
	int getLocalPropID(Prop * p);

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
};