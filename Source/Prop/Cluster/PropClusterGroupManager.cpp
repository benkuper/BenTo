/*
  ==============================================================================

    PropClusterGroupManager.cpp
    Created: 5 Feb 2019 11:53:33am
    Author:  jonglissimo

  ==============================================================================
*/

#include "BentoEngine.h"

PropClusterGroupManager::PropClusterGroupManager() :
	Manager("Cluster Groups")
{
	selectItemWhenCreated = false;
	PropManager::getInstance()->addControllableContainerListener(this);
}

PropClusterGroupManager::~PropClusterGroupManager()
{
	if(PropManager::getInstanceWithoutCreating() != nullptr) PropManager::getInstance()->removeControllableContainerListener(this);
}