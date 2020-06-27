/*
  ==============================================================================

    LightBlockModelLibrary.h
    Created: 10 Apr 2018 10:57:18pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "UserLightBlockModelManager.h"
#include "LightBlockModelGroup.h"

class LightBlockModelLibrary :
	public ControllableContainer
{
public:
	juce_DeclareSingleton(LightBlockModelLibrary, true)
	LightBlockModelLibrary();
	~LightBlockModelLibrary();
	
	LightBlockModelGroup genericBlocks;
	LightBlockModelGroup liveFeedBlocks;
	UserLightBlockModelManager pictureBlocks;	
	UserLightBlockModelManager nodeBlocks;
	UserLightBlockModelManager scriptBlocks;
	UserLightBlockModelManager timelineBlocks;


	//Generic / Patterns
	std::unique_ptr<LightBlockModel> solidColorBlock;
	std::unique_ptr<LightBlockModel> rainbowBlock;
	std::unique_ptr<LightBlockModel> strobeBlock;
	std::unique_ptr<LightBlockModel> noiseBlock;
	std::unique_ptr<LightBlockModel> pointBlock;
	std::unique_ptr<LightBlockModel> multiPointBlock;

	//Generic / Filters
	LightBlockModelGroup genericFilterBlocks;
	std::unique_ptr<LightBlockModel> remapBlock;
	std::unique_ptr<LightBlockModel> hsvBlock;
	std::unique_ptr<LightBlockModel> brightContrastBlock;

	//Live Feed
	std::unique_ptr<LightBlockModel> videoBlock;
	std::unique_ptr<LightBlockModel> shapeBlock;
	std::unique_ptr<LightBlockModel> dmxBlock;

	//Interface
	IntParameter * iconSize;

	void clear() override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	Array<LightBlockModel*> getAllModels(bool includeUserModels = true);
	Array<LightBlockModel *> getAllFilters(bool includeUserModels = true);
	LightBlockModel* getModelWithName(const String& modelName);
	LightBlockModel * getFilterWithName(const String &modelName);

	static LightBlockColorProvider* showSourcesAndGet();
	static LightBlockColorProvider * showFiltersAndGet();
	static Array<LightBlockColorProvider *> fillProvidersMenu(PopupMenu &menu, bool includePresets, bool includeSources, bool includeFilters, int startIndex = 1);
	static Array<LightBlockColorProvider *> fillUserLightBlockManagerMenu(UserLightBlockModelManager * manager, PopupMenu &menu, bool includePresets, int startIndex);

};