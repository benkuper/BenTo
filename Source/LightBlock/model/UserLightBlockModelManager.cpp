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
#include "blocks/picture/PictureBlock.h"

UserLightBlockModelManager::UserLightBlockModelManager(const String &name, BlockType type) :
	BaseManager(name),
	type(type)
{
	libFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Bento/" + name);
	libFolder.createDirectory();
}

UserLightBlockModelManager::~UserLightBlockModelManager()
{
	
}

LightBlockModel * UserLightBlockModelManager::createItem()
{
	switch (type)
	{
	case PICTURE: return new PictureBlock();
	case NODE: return new NodeBlock();
	case SCRIPT: return new ScriptBlock();
	case TIMELINE: return new TimelineBlock();

	default:
		break;
	}
	jassertfalse;
	return nullptr;
}
