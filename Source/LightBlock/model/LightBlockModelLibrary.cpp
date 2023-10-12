/*
  ==============================================================================

	LightBlockModelLibrary.cpp
	Created: 10 Apr 2018 10:57:18pm
	Author:  Ben

  ==============================================================================
*/


#include "LightBlock/LightBlockIncludes.h"

juce_ImplementSingleton(LightBlockModelLibrary)

LightBlockModelLibrary::LightBlockModelLibrary() :
	ControllableContainer("Library"),
	patternBlocks("Patterns"),
	//videoBlocks("Live Feed", UserLightBlockModelManager::LIVE_FEED),
	videoBlocks("Video", UserLightBlockModelManager::VIDEO),
	pictureBlocks("Pictures", UserLightBlockModelManager::PICTURE),
	nodeBlocks("Nodes", UserLightBlockModelManager::NODE),
	streamingScriptBlocks("Streaming Scripts", UserLightBlockModelManager::STREAMING_SCRIPT),
	embeddedScriptBlocks("Embedded Scripts", UserLightBlockModelManager::EMBEDDED_SCRIPT),
	sequenceBlocks("Sequences", UserLightBlockModelManager::SEQUENCE)
	//genericFilterBlocks("Generic Filters")
{

	streamingScriptBlocks.itemDataType = "StreaminScript";
	embeddedScriptBlocks.itemDataType = "EmbeddedScript";
	pictureBlocks.itemDataType = "Picture";
	//videoBlocks.itemDataType = "SharedTexture";
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

	//VideoFileBlock.reset(new VideoFileBlock());;
	//shapeBlock.reset(new ShapeEditorBlock());;
	//dmxBlock.reset(new DMXBlock());

	patternBlocks.addChildControllableContainer(solidColorBlock.get());
	patternBlocks.addChildControllableContainer(rainbowBlock.get());
	patternBlocks.addChildControllableContainer(strobeBlock.get());
	patternBlocks.addChildControllableContainer(noiseBlock.get());
	patternBlocks.addChildControllableContainer(pointBlock.get());
	patternBlocks.addChildControllableContainer(multiPointBlock.get());
	patternBlocks.addChildControllableContainer(rangeBlock.get());

	addChildControllableContainer(&patternBlocks);

	//videoBlocks.addChildControllableContainer(VideoFileBlock.get());
	//videoBlocks.addChildControllableContainer(shapeBlock.get());
	//videoBlocks.addChildControllableContainer(dmxBlock.get());
	//addChildControllableContainer(&videoBlocks);
	addChildControllableContainer(&videoBlocks);

	addChildControllableContainer(&pictureBlocks);
	addChildControllableContainer(&nodeBlocks);
	addChildControllableContainer(&streamingScriptBlocks);
	addChildControllableContainer(&embeddedScriptBlocks);
	addChildControllableContainer(&sequenceBlocks);

	/*remapBlock.reset(new RemapBlockFilter());
	hsvBlock.reset(new HSVBlockFilter());
	brightContrastBlock.reset(new BrightnessContrastBlockFilter());*/

	//addChildControllableContainer(&genericFilterBlocks);
	//genericFilterBlocks.addChildControllableContainer(remapBlock.get());
	//genericFilterBlocks.addChildControllableContainer(hsvBlock.get());
	//genericFilterBlocks.addChildControllableContainer(brightContrastBlock.get());

	iconSize = addIntParameter("Icon size", "Size of library icons", 70, 16, 120);
	iconSize->hideInEditor = true;
}

LightBlockModelLibrary::~LightBlockModelLibrary()
{

}


void LightBlockModelLibrary::clear()
{
	Array<LightBlockModel*> models = getAllModels(false);
	sequenceBlocks.clear();
	videoBlocks.clear();
	streamingScriptBlocks.clear();
	embeddedScriptBlocks.clear();
	for (auto& m : models) m->clear();
	pictureBlocks.clear();
	nodeBlocks.clear();
}

var LightBlockModelLibrary::getJSONData()
{
	var data = ControllableContainer::getJSONData();

	var gData = patternBlocks.getJSONData();
	if (gData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(patternBlocks.shortName, gData);

	var uData = pictureBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(pictureBlocks.shortName, uData);
	uData = nodeBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(nodeBlocks.shortName, uData);
	uData = streamingScriptBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(streamingScriptBlocks.shortName, uData);
	uData = embeddedScriptBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(embeddedScriptBlocks.shortName, uData);
	uData = sequenceBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(sequenceBlocks.shortName, uData);
	uData = videoBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(videoBlocks.shortName, uData);
	
	/*var fData = genericFilterBlocks.getJSONData();
	if (fData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("genericFilters", fData);*/

	return data;
}

void LightBlockModelLibrary::loadJSONDataInternal(var data)
{
	patternBlocks.loadJSONData(data.getProperty(patternBlocks.shortName, var()));
	videoBlocks.loadJSONData(data.getProperty(videoBlocks.shortName, var()));
	pictureBlocks.loadJSONData(data.getProperty(pictureBlocks.shortName, var()));
	nodeBlocks.loadJSONData(data.getProperty(nodeBlocks.shortName, var()));
	streamingScriptBlocks.loadJSONData(data.getProperty(streamingScriptBlocks.shortName, var()));
	embeddedScriptBlocks.loadJSONData(data.getProperty(embeddedScriptBlocks.shortName, var()));
	sequenceBlocks.loadJSONData(data.getProperty(sequenceBlocks.shortName, var()));
	//genericFilterBlocks.loadJSONData(data.getProperty("genericFilters", var()));
}

Array<LightBlockModel*> LightBlockModelLibrary::getAllModels(bool includeUserModels)
{
	Array<LightBlockModel*> result;

	for (auto& cc : patternBlocks.controllableContainers)
	{
		LightBlockModel* m = dynamic_cast<LightBlockModel*>(cc.get());
		if (m == nullptr) continue;
		result.add(m);
	}


	if (includeUserModels)
	{
		for (auto& m : pictureBlocks.items) result.add(m);
		for (auto& m : nodeBlocks.items) result.add(m);
		for (auto& m : streamingScriptBlocks.items) result.add(m);
		for (auto& m : embeddedScriptBlocks.items) result.add(m);
		for (auto& m : sequenceBlocks.items) result.add(m);
		for (auto& m : videoBlocks.items) result.add(m);
		for (auto& m : videoBlocks.items) result.add(m);
	}

	return result;
}

Array<LightBlockModel*> LightBlockModelLibrary::getAllFilters(bool includeUserModels)
{
	Array<LightBlockModel*> result;

	//for (auto& cc : genericFilterBlocks.controllableContainers)
	//{
	//	LightBlockModel* m = dynamic_cast<LightBlockModel*>(cc.get());
	//	if (m == nullptr) continue;
	//	result.add(m);
	//}

	return result;
}

LightBlockModel* LightBlockModelLibrary::getModelWithName(const String& modelName)
{
	Array<LightBlockModel*> models = getAllModels();
	for (auto& m : models) if (m->shortName == modelName || m->niceName == modelName) return m;
	return nullptr;
}

LightBlockModel* LightBlockModelLibrary::getFilterWithName(const String& modelName)
{
	Array<LightBlockModel*> models = getAllFilters();
	for (auto& m : models) if (m->shortName == modelName) return m;
	return nullptr;
}

void LightBlockModelLibrary::showSourcesAndGet(ControllableContainer* sourceCC, std::function<void(ControllableContainer*)> returnFunc)
{
	PopupMenu menu;
	Array<LightBlockColorProvider*> mList = fillProvidersMenu(menu, true, true, false);

	menu.showMenuAsync(PopupMenu::Options(), [&mList, returnFunc](int result)
		{
			if (result > 0) returnFunc(mList[result - 1]);
		}
	);
}


void LightBlockModelLibrary::showFiltersAndGet(ControllableContainer* sourceCC, std::function<void(ControllableContainer*)> returnFunc)
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

	Array<LightBlockColorProvider*> mList;


	int index = startIndex;

	if (includeSources)
	{

		for (auto& cc : LightBlockModelLibrary::getInstance()->patternBlocks.controllableContainers)
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

		//for (auto& cc : LightBlockModelLibrary::getInstance()->videoBlocks.controllableContainers)
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

		//		SharedTextureMenu.addSubMenu(m->niceName, modelMenu);
		//	}
		//	else
		//	{
		//		SharedTextureMenu.addItem(index, m->niceName);
		//		mList.add(m);
		//		index++;
		//	}
		//}


		menu.addSubMenu(LightBlockModelLibrary::getInstance()->patternBlocks.niceName, genericMenu);

		PopupMenu SharedTexturesMenu;
		Array<LightBlockColorProvider*> pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->videoBlocks, SharedTexturesMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->pictureBlocks.niceName, SharedTexturesMenu);

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
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->streamingScriptBlocks, scriptsMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->streamingScriptBlocks.niceName, scriptsMenu);

		PopupMenu wasmMenu;
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->embeddedScriptBlocks, wasmMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->embeddedScriptBlocks.niceName, wasmMenu);


		PopupMenu sequencesMenu;
		pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->sequenceBlocks, sequencesMenu, includePresets, index);
		index += pa.size();
		mList.addArray(pa);
		menu.addSubMenu(LightBlockModelLibrary::getInstance()->sequenceBlocks.niceName, sequencesMenu);
	}

	//if (includeFilters)
	//{
	//	for (auto& cc : LightBlockModelLibrary::getInstance()->genericFilterBlocks.controllableContainers)
	//	{
	//		LightBlockModel* m = dynamic_cast<LightBlockModel*>(cc.get());
	//		if (m == nullptr) continue;

	//		if (includePresets)
	//		{
	//			PopupMenu modelMenu;

	//			modelMenu.addItem(index, "Default");
	//			mList.add(m);
	//			index++;

	//			for (auto& mp : m->presetManager.items)
	//			{
	//				modelMenu.addItem(index, mp->niceName);
	//				mList.add(mp);
	//				index++;
	//			}

	//			genericMenu.addSubMenu(m->niceName, modelMenu);
	//		}
	//		else
	//		{
	//			genericMenu.addItem(index, m->niceName);
	//			mList.add(m);
	//			index++;
	//		}
	//	}

	//}

	return mList;
}

Array<LightBlockColorProvider*> LightBlockModelLibrary::fillUserLightBlockManagerMenu(UserLightBlockModelManager* manager, PopupMenu& menu, bool includePresets, int startIndex)
{
	Array<LightBlockColorProvider*> mList;

	int index = startIndex;
	for (auto& m : manager->items)
	{
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

			menu.addSubMenu(m->niceName, modelMenu);
		}
		else
		{
			menu.addItem(index, m->niceName);
			mList.add(m);
			index++;
		}
	}

	return mList;
}
