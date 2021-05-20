/*
  ==============================================================================

    LightBlockModelPresetManager.h
    Created: 12 Apr 2018 1:12:10pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockModel;

class LightBlockModelPresetManager :
	public BaseManager<LightBlockModelPreset>
{
public:
	LightBlockModelPresetManager(LightBlockModel * model);
	~LightBlockModelPresetManager();

	LightBlockModel * model;
	LightBlockModelPreset * createItem() override;
};