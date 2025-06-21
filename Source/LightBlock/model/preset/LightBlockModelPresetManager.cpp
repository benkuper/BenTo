/*
  ==============================================================================

	LightBlockModelPresetManager.cpp
	Created: 12 Apr 2018 1:12:10pm
	Author:  Ben

  ==============================================================================
*/

LightBlockModelPresetManager::LightBlockModelPresetManager(LightBlockModel * model) :
	Manager("Presets"),
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
