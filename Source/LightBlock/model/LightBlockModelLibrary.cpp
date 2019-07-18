/*
  ==============================================================================

	LightBlockModelLibrary.cpp
	Created: 10 Apr 2018 10:57:18pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelLibrary.h"
#include "blocks/timeline/TimelineBlock.h"
#include "blocks/pattern/PatternBlock.h"
#include "blocks/video/VideoBlock.h"
#include "blocks/dmx/DMXBlock.h"
#include "blocks/picture/PictureBlock.h"

juce_ImplementSingleton(LightBlockModelLibrary)


LightBlockModelLibrary::LightBlockModelLibrary() :
	ControllableContainer("Library"),
	genericBlocks("Generic"),
	liveFeedBlocks("Live Feed"),
	pictureBlocks("Pictures", UserLightBlockModelManager::PICTURE),
	nodeBlocks("Nodes", UserLightBlockModelManager::NODE),
	scriptBlocks("Scripts", UserLightBlockModelManager::SCRIPT),
	timelineBlocks("Timeline", UserLightBlockModelManager::TIMELINE)
{
	//patterns
	solidColorBlock.reset(new SolidColorPattern());
	rainbowBlock.reset(new RainbowPattern());
	strobeBlock.reset(new StrobePattern());
	noiseBlock.reset(new NoisePattern());
	pointBlock.reset(new PointPattern());
	multiPointBlock.reset(new MultiPointPattern());

	videoBlock.reset(new VideoBlock());
	dmxBlock.reset(new DMXBlock());

	genericBlocks.addChildControllableContainer(solidColorBlock.get());
	genericBlocks.addChildControllableContainer(rainbowBlock.get());
	genericBlocks.addChildControllableContainer(strobeBlock.get());
	genericBlocks.addChildControllableContainer(noiseBlock.get());
	genericBlocks.addChildControllableContainer(pointBlock.get());
	genericBlocks.addChildControllableContainer(multiPointBlock.get());

	addChildControllableContainer(&genericBlocks);

	liveFeedBlocks.addChildControllableContainer(videoBlock.get());
	liveFeedBlocks.addChildControllableContainer(dmxBlock.get());
	addChildControllableContainer(&liveFeedBlocks);

	addChildControllableContainer(&pictureBlocks);
	addChildControllableContainer(&nodeBlocks);
	addChildControllableContainer(&scriptBlocks);
	addChildControllableContainer(&timelineBlocks);

	iconSize = addIntParameter("Icon size", "Size of library icons", 70, 16, 120);
}

LightBlockModelLibrary::~LightBlockModelLibrary()
{

}


void LightBlockModelLibrary::clear()
{
	Array<LightBlockModel *> models = getAllModels(false);
	timelineBlocks.clear();
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
	var lData = liveFeedBlocks.getJSONData();
	if (lData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("liveFeeds", lData);
	
	var uData = pictureBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("pictures", uData);
	uData = nodeBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("nodes", uData);
	uData = scriptBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("scripts", uData);
	uData = timelineBlocks.getJSONData();
	if (uData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("timelines", uData);


	return data;
}

void LightBlockModelLibrary::loadJSONDataInternal(var data)
{
	genericBlocks.loadJSONData(data.getProperty("generic", var()));
	liveFeedBlocks.loadJSONData(data.getProperty("liveFeeds", var()));
	pictureBlocks.loadJSONData(data.getProperty("pictures", var()));
	nodeBlocks.loadJSONData(data.getProperty("nodes", var()));
	scriptBlocks.loadJSONData(data.getProperty("scripts", var()));
	timelineBlocks.loadJSONData(data.getProperty("timelines", var()));
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

	for (auto &cc : liveFeedBlocks.controllableContainers)
	{
		LightBlockModel * m = dynamic_cast<LightBlockModel *>(cc.get());
		if (m == nullptr) continue;
		result.add(m);
	}

	if (includeUserModels)
	{
		for (auto & m : pictureBlocks.items) result.add(m);
		for (auto & m : nodeBlocks.items) result.add(m);
		for (auto & m : scriptBlocks.items) result.add(m);
		for (auto & m : timelineBlocks.items) result.add(m);
	}

	return result;
}

LightBlockModel * LightBlockModelLibrary::getModelWithName(const String & modelName)
{
	Array<LightBlockModel *> models = getAllModels();
	for (auto &m : models) if (m->shortName == modelName) return m;
	return nullptr;
}

LightBlockColorProvider * LightBlockModelLibrary::showProvidersAndGet()
{
	return showAllModelsAndGet(true);
}

LightBlockColorProvider * LightBlockModelLibrary::showAllModelsAndGet(bool includePresets)
{
	PopupMenu menu;
	Array<LightBlockColorProvider *> mList = fillProvidersMenu(menu, includePresets);
	int result = menu.show();

	if (result > 0) return mList[result - 1];
	return nullptr;
}

Array<LightBlockColorProvider *> LightBlockModelLibrary::fillProvidersMenu(PopupMenu &menu, bool includePresets, int startIndex)
{
	PopupMenu genericMenu;
	PopupMenu liveFeedMenu;
	PopupMenu userMenu;

	Array<LightBlockColorProvider *> mList;

	int index = startIndex;
	for (auto &cc : LightBlockModelLibrary::getInstance()->genericBlocks.controllableContainers)
	{
		LightBlockModel * m = dynamic_cast<LightBlockModel *>(cc.get());
		if (m == nullptr) continue;

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

			genericMenu.addSubMenu(m->niceName, modelMenu);
		} else
		{
			genericMenu.addItem(index, m->niceName);
			mList.add(m);
			index++;
		}
	}

	for (auto &cc : LightBlockModelLibrary::getInstance()->liveFeedBlocks.controllableContainers)
	{
		LightBlockModel * m = dynamic_cast<LightBlockModel *>(cc.get());
		if (m == nullptr) continue;

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

			liveFeedMenu.addSubMenu(m->niceName, modelMenu);
		} else
		{
			liveFeedMenu.addItem(index, m->niceName);
			mList.add(m);
			index++;
		}
	}

	
	menu.addSubMenu(LightBlockModelLibrary::getInstance()->genericBlocks.niceName, genericMenu);
	menu.addSubMenu(LightBlockModelLibrary::getInstance()->liveFeedBlocks.niceName, liveFeedMenu);
	
	PopupMenu picturesMenu;
	Array<LightBlockColorProvider *> pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->pictureBlocks, picturesMenu, includePresets, index);
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

	PopupMenu timelinesMenu;
	pa = fillUserLightBlockManagerMenu(&LightBlockModelLibrary::getInstance()->timelineBlocks, timelinesMenu, includePresets, index);
	index += pa.size();
	mList.addArray(pa);
	menu.addSubMenu(LightBlockModelLibrary::getInstance()->timelineBlocks.niceName, timelinesMenu);

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
