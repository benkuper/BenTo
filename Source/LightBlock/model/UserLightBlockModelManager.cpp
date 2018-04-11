/*
  ==============================================================================

    UserLightBlockModelManager.cpp
    Created: 10 Apr 2018 7:48:55pm
    Author:  Ben

  ==============================================================================
*/

#include "UserLightBlockModelManager.h"
#include "blocks/node/NodeBlock.h"
#include "blocks/script/ScriptBlock.h"
#include "blocks/timeline/TimelineBlock.h"


UserLightBlockModelManager::UserLightBlockModelManager() :
	BaseManager("Blocks")
{
	itemDataType = "LightBlockModel";
	managerFactory = &factory;

	factory.defs.add(Factory<LightBlockModel>::Definition::createDef("", "Timeline", &TimelineBlock::create));
	factory.defs.add(Factory<LightBlockModel>::Definition::createDef("", "Node", &NodeBlock::create));
	factory.defs.add(Factory<LightBlockModel>::Definition::createDef("", "Script", &ScriptBlock::create));
}

UserLightBlockModelManager::~UserLightBlockModelManager()
{

}
