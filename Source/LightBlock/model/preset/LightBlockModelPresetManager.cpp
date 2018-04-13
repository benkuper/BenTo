/*
  ==============================================================================

	LightBlockModelPresetManager.cpp
	Created: 12 Apr 2018 1:12:10pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelPresetManager.h"

LightBlockModelPresetManager::LightBlockModelPresetManager(LightBlockModel * model) :
	BaseManager("Presets"),
	model(model)
{
}

LightBlockModelPresetManager::~LightBlockModelPresetManager()
{
}

LightBlockModelPreset * LightBlockModelPresetManager::createItem()
{
	return new LightBlockModelPreset(model);
}
