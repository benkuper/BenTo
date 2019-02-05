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

	BaseManager<PropCluster> clusterManager;

	int getLocalPropID(Prop * p);
};