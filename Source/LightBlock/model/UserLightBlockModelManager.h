/*
  ==============================================================================

    UserLightBlockModelManager.h
    Created: 10 Apr 2018 7:48:55pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlockModel.h"

class UserLightBlockModelManager :
	public BaseManager<LightBlockModel>
{
public:

	enum BlockType { PICTURE, NODE, SCRIPT, TIMELINE };

	UserLightBlockModelManager(const String &name, BlockType type);
	~UserLightBlockModelManager();

	BlockType type;
	File libFolder;

	LightBlockModel * createItem();
};