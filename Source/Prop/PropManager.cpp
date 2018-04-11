/*
  ==============================================================================

    PropManager.cpp
    Created: 10 Apr 2018 6:59:29pm
    Author:  Ben

  ==============================================================================
*/

#include "PropManager.h"
juce_ImplementSingleton(PropManager)

#include "props/balls/flowball/FlowBallProp.h"
#include "props/clubs/flowclub/FlowClubProp.h"
#include "props/pois/flowpoi/FlowPoiProp.h"

PropManager::PropManager() :
	BaseManager("Props")
{
	managerFactory = &factory;
	factory.defs.add(Factory<Prop>::Definition::createDef("Flowtoys", "FlowClub", FlowClubProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Flowtoys", "FlowPoi", FlowPoiProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Flowtoys", "FlowBall", FlowBallProp::create));
}

PropManager::~PropManager()
{
}
