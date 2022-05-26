/*
  ==============================================================================

	LightBlockModelLibrary.cpp
	Created: 10 Apr 2018 10:57:18pm
	Author:  Ben

  ==============================================================================
*/

juce_ImplementSingleton(LightBlockModelLibrary)


LightBlockModelLibrary::LightBlockModelLibrary() :
	ControllableContainer("Library"),
	genericBlocks("Generic"),
	liveFeedBlocks("Live Feed", UserLightBlockModelManager::LIVE_FEED),
	videoBlocks("Video", UserLightBlockModelManager::VIDEO),
	pictureBlocks("Pictures", UserLightBlockModelManager::PICTURE),
	nodeBlocks("Nodes", UserLightBlockModelManager::NODE),
	scriptBlocks("Scripts", UserLightBlockModelManager::SCRIPT),
	timelineBlocks("Timeline", UserLightBlockModelManager::TIMELINE),
	genericFilterBlocks("Generic Filters")
{

	scriptBlocks.itemDataType = "Script";
	scriptBlocks.itemDataType = "Wasm";
	pictureBlocks.itemDataType = "Picture";
	liveFeedBlocks.itemDataType = "LiveFeed";
	videoBlocks.itemDataType = "Video";
	nodeBlocks.itemDataType = "Node";

	//patterns
	solidColorBlock.reset(new SolidColorPattern());
	rainbowBlock.reset(new RainbowPattern());
	strobeBlock.reset(new StrobePattern());
	noiseBlock.reset(new NoisePattern());
	pointBlock.reset(new PointPattern());
	multiPointBlock.reset(new MultiPointPattern());
	rangeBlock.reset(new RangePattern());

	//videoBlock.reset(new VideoBlock());;
	//shapeBlock.reset(new ShapeEditorBlock());;
	//dmxBlock.reset(new DMXBlock());

	genericBlocks.addChildControllableContainer(solidColorBlock.get());
	genericBlocks.addChildControllableContainer(rainbowBlock.get());
	genericBlocks.addChildControllableContainer(strobeBlock.get());
	genericBlocks.addChildControllableContainer(noiseBlock.get());
	genericBlocks.addChildControllableContainer(pointBlock.get());
	genericBlocks.addChildControllableContainer(multiPointBlock.get());
	genericBlocks.addChildControllableContainer(rangeBlock.get());

	addChildControllableContainer(&genericBlocks);

	//liveFeedBlocks.addChildControllableContainer(videoBlock.get());
	//liveFeedBlocks.addChildControllableContainer(shapeBlock.get());
	//liveFeedBlocks.addChildControllableContainer(dmxBlock.get());
	addChildControllableContainer(&liveFeedBlocks);
	addChildControllableContainer(&videoBlocks);

	addChildControllableContainer(&pictureBlocks);
	addChildControllableContainer(&nodeBlocks);
	addChildControllableContainer(&scriptBlocks);
	addChildControllableContainer(&wasmBlocks);
	addChildControllableContainer(&timelineBlocks);



	remapBlock.reset(new RemapBlockFilter());
	hsvBlock.reset(new HSVBlockFilter());
	brightContrastBlock.reset(new BrightnessContrastBlockFilter());

	addChildControllableContainer(&genericFilterBlocks);
	genericFilterBlocks.addChildControllableContainer(remapBlock.get());
	genericFilterBlocks.addChildControllableContainer(hsvBlock.get());
	genericFilterBlocks.addChildControllableContainer(brightContrastBlock.get());

	iconSize = addIntParameter("Icon size", "Size of library icons", 70, 16, 120);
}

LightBlockModelLibrary::~LightBlockModelLibrary()
{

}


void LightBlockModelLibrary::clear()
{
	Array<LightBlockModel *> models = getAllModels(false);
	timelineBlocks.clear();
	liveFeedBlocks.clear();
	videoBlocks.clear();
	scriptBlocks.clear();
	for (auto &m : models) m->clear();
	pictureBlocks.clear();
	nodeBlocks.clear();
}

var LightBlockModelLibrary::getJSONData()
{
	var data = ControllableContainer::getJSONData();

	var gData = genericBlocks.getJSONData();
	if(gData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("generic", gData);
	
	var uData = pictureBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("pictures", uData);
	uData = nodeBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("nodes", uData);
	uData = scriptBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("scripts", uData);
	uData = wasmBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("wasm", uData);
	uData = timelineBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("timelines", uData);
	uData = liveFeedBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("liveFeeds", uData);
	uData = videoBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("video", uData);


	var fData = genericFilterBlocks.getJSONData();
	if (fData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("genericFilters", fData);

	return data;
}

void LightBlockModelLibrary::loadJSONDataInternal(var data)
{
	genericBlocks.loadJSONData(data.getProperty("generic", var()));
	liveFeedBlocks.loadJSONData(data.getProperty("liveFeeds", var()));
	videoBlocks.loadJSONData(data.getProperty("video", var()));
	pictureBlocks.loadJSONData(data.getProperty("pictures", var()));
	nodeBlocks.loadJSONData(data.getProperty("nodes", var()));
	scriptBlocks.loadJSONData(data.getProperty("scripts", var()));
	wasmBlocks.loadJSONData(data.getProperty("wasm", var()));
	timelineBlocks.loadJSONData(data.getProperty("timelines", var()));
	genericFilterBlocks.loadJSONData(data.getProperty("genericFilters", var()));
}

Array<LightBlockModel*> LightBlockModelLibrary::getAllModels(bool includeUserModels)
{
	Array<LightBlockModel*> result;

	for (auto &cc : genericBlocks.controllableContainers)
	{
		LightBlockModel * m = dynamic_cast<LightBlockModel *>(cc.get());
		if (m == nullptr) continue;
		result.add(m);
	}


	if (includeUserModels)
	{
		for (auto & m : pictureBlocks.items) result.add(m);
		for (auto & m : nodeBlocks.items) result.add(m);
		for (auto& m : scriptBlocks.items) result.add(m);
		for (auto & m : wasmBlocks.items) result.add(m);
		for (auto& m : timelineBlocks.items) result.add(m);
		for (auto& m : liveFeedBlocks.items) result.add(m);
		for (auto & m : videoBlocks.items) result.add(m);
	}

	return result;
}

Array<LightBlockModel*> LightBlockModelLibrary::getAllFilters(bool includeUserModels)
{
	Array<LightBlockModel*> result;

	for (auto& cc : genericFilterBlocks.controllableContainers)
	{
		LightBlockModel* m = dynamic_cast<LightBlockModel*>(cc.get());
		if (m == nullptr) continue;
		result.add(m);
	}

	return result;
}

LightBlockModel * LightBlockModelLibrary::getModelWithName(const String & modelName)
{
	Array<LightBlockModel *> models = getAllModels();
	for (auto &m : models) if (m->shortName == modelName || m->niceName == modelName) return m;
	return nullptr;
}

LightBlockModel* LightBlockModelLibrary::getFilterWithName(const String& modelName)
{
	Array<LightBlockModel*> models = getAllFilters();
	for (auto& m : models) if (m->shortName == modelName) return m;
	return nullptr;
}

void LightBlockModelLibrary::showSourcesAndGet(std::function<void(ControllableContainer*)> returnFunc)
{
	PopupMenu menu;
	Array<LightBlockColorProvider *> mList = fillProvidersMenu(menu, true, true, false);
	
	menu.showMenuAsync(PopupMenu::Options(), [&mList, returnFunc](int result)
		{
			if (result > 0) returnFunc(mList[result - 1]);
		}
	);
}


void LightBlockModelLibrary::showFiltersAndGet(std::function<void(ControllableContainer*)> returnFunc)
{
	PopupMenu menu;
	Array<LightBlockColorProvider*> mList = fillProvidersMenu(menu, true, false, true);
	menu.showMenuAsync(PopupMenu::Options(), [&mList, returnFunc](int result)
		{
			if (result > 0) returnFunc(mList[result - 1]);
		}
	);
}

Array<LightBlockColorProvider*> LightBlockModelLibrary::fillProvidersMenu(PopupMenu& menu, bool includePresets, bool includeSources, bool includeFilters, int startIndex)
{
	PopupMenu genericMenu;
	PopupMenu userMenu;

	Array<LightBlockColorProvider *> mList;


	int index = startIndex;

	if (includeSources)
	{

		for (auto& cc : LightBlockModelLibrary::getInstance()->genericBlocks.controllableContainers)
		{
			LightBlockModel* m = dynamic_cast<LightBlockModel*>(cc.get());
			if (m == nullptr) continue;

			if (includePresets)
			{
				PopupMenu modelMenu;

				modelMenu.addItem(index, "Default");
				mList.add(m);
				index++;

				for (auto& mp : m->presetManager.items)
				{
					modelMenu.addItem(index, mp->niceName);
					mList.add(mp);
					index++;
				}

				genericMenu.addSubMenu(m->niceName, modelMenu);
			}
			else
			{
				genericMenu.addItem(index, m->niceName);
				mList.add(m);
				index++;
			}
		}

		//for (auto& cc : LightBlockModelLibrary::getInstance()->liveFeedBlocks.controllableContainers)
		//{
		//	LightBlockModel* m = dynamic_cast<LightBlockModel*>(cc.get());
		//	if (m == nullptr) continue;

		//	if (includePresets)
		//	{
		//		PopupMenu modelMenu;

		//		modelMenu.addItem(index, "Default");
		//		mList.add(m);
		//		index++;

		//		for (auto& mp : m->presetManager.items)
		//		{
		//			modelMenu.addItem(index, mp->niceName);
		//			mList.add(mp);
		//			index++;
		//		}

		//		liveFeedMenu.addSubMenu(m->niceName, modelMenu);
		//	}
		//	else
		//	{
		//		liveFeedMenu.addItem(index, m->niceName);
		//		mList.add(m);
		//		index++;
		//	}
		//}


		menu.addSubMenu(LightBlockModelLibrary::getInstance()->genericBlocks.niceName, genericMenu);

		PopupMenu liveFeedsMenu;
		Array<LightBlockColorProvider*> pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->liveFeedBlocks, liveFeedsMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->pictureBlocks.niceName, liveFeedsMenu);

		PopupMenu videoMenu;
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->videoBlocks, videoMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->pictureBlocks.niceName, videoMenu);

		PopupMenu picturesMenu;
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->pictureBlocks, picturesMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->pictureBlocks.niceName, picturesMenu);


		PopupMenu nodesMenu;
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->nodeBlocks, nodesMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->nodeBlocks.niceName, nodesMenu);

		PopupMenu scriptsMenu;
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->scriptBlocks, scriptsMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->scriptBlocks.niceName, scriptsMenu);

		PopupMenu wasmMenu;
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->wasmBlocks, wasmMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->wasmBlocks.niceName, wasmMenu);


		PopupMenu timelinesMenu;
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->timelineBlocks, timelinesMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->timelineBlocks.niceName, timelinesMenu);
	}

	if (includeFilters)
	{
		for (auto& cc : LightBlockModelLibrary::getInstance()->genericFilterBlocks.controllableContainers)
		{
			LightBlockModel* m = dynamic_cast<LightBlockModel*>(cc.get());
			if (m == nullptr) continue;

			if (includePresets)
			{
				PopupMenu modelMenu;

				modelMenu.addItem(index, "Default");
				mList.add(m);
				index++;

				for (auto& mp : m->presetManager.items)
				{
					modelMenu.addItem(index, mp->niceName);
					mList.add(mp);
					index++;
				}

				genericMenu.addSubMenu(m->niceName, modelMenu);
			}
			else
			{
				genericMenu.addItem(index, m->niceName);
				mList.add(m);
				index++;
			}
		}

	}

	return mList;
}

Array<LightBlockColorProvider*> LightBlockModelLibrary::fillUserLightBlockManagerMenu(UserLightBlockModelManager * manager, PopupMenu & menu, bool includePresets, int startIndex)
{
	Array<LightBlockColorProvider*> mList;

	int index = startIndex;
	for (auto & m : manager->items)
	{
		if (includePresets)
		{
			PopupMenu modelMenu;

			modelMenu.addItem(index, "Default");
			mList.add(m);
			index++;
			for (auto &mp : m->presetManager.items)
			{
				modelMenu.addItem(index, mp->niceName);
				mList.add(mp);
				index++;
			}

			menu.addSubMenu(m->niceName, modelMenu);
		} else
		{
			menu.addItem(index, m->niceName);
			mList.add(m);
			index++;
		}
	}

	return mList;
}
