/*
  ==============================================================================

    PropClusterGroupManager.cpp
    Created: 5 Feb 2019 11:53:33am
    Author:  jonglissimo

  ==============================================================================
*/

#include "PropClusterGroupManager.h"

juce_ImplementSingleton(PropClusterGroupManager)

PropClusterGroupManager::PropClusterGroupManager() :
	BaseManager("Cluster Groups")
{
}

PropClusterGroupManager::~PropClusterGroupManager()
{
}
