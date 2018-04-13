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

juce_ImplementSingleton(LightBlockModelLibrary)


LightBlockModelLibrary::LightBlockModelLibrary() :
	ControllableContainer("Block Library"),
	genericBlocks("Generic"),
	liveFeedBlocks("Live Feed")
{
	genericBlocks.saveAndLoadRecursiveData = true;
	liveFeedBlocks.saveAndLoadRecursiveData = true;

	//patterns
	solidColorBlock = new SolidColorPattern();
	rainbowBlock = new RainbowPattern();
	strobeBlock = new StrobePattern();
	noiseBlock = new NoisePattern();

	videoBlock = new VideoBlock();
	dmxBlock = new DMXBlock();

	genericBlocks.addChildControllableContainer(solidColorBlock);
	genericBlocks.addChildControllableContainer(rainbowBlock);
	genericBlocks.addChildControllableContainer(strobeBlock);
	genericBlocks.addChildControllableContainer(noiseBlock);
	addChildControllableContainer(&genericBlocks);

	liveFeedBlocks.addChildControllableContainer(videoBlock);
	liveFeedBlocks.addChildControllableContainer(dmxBlock);
	addChildControllableContainer(&liveFeedBlocks);

	addChildControllableContainer(&userBlocks);
}

LightBlockModelLibrary::~LightBlockModelLibrary()
{

}


void LightBlockModelLibrary::clear()
{
	Array<LightBlockModel *> models = getAllModels(false);
	for (auto &m : models) m->clear();
	userBlocks.clear();
}

var LightBlockModelLibrary::getJSONData()
{
	var data = ControllableContainer::getJSONData();
	data.getDynamicObject()->setProperty("generic", genericBlocks.getJSONData());
	data.getDynamicObject()->setProperty("liveFeeds", liveFeedBlocks.getJSONData());
	data.getDynamicObject()->setProperty("userBlocks", userBlocks.getJSONData());

	return data;
}

void LightBlockModelLibrary::loadJSONDataInternal(var data)
{
	genericBlocks.loadJSONData(data.getProperty("generic", var()));
	liveFeedBlocks.loadJSONData(data.getProperty("liveFeeds", var()));
	userBlocks.loadJSONData(data.getProperty("userBlocks", var()));
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
		for (auto & m : userBlocks.items) result.add(m);
	}

	return result;
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

	for (auto & m : LightBlockModelLibrary::getInstance()->userBlocks.items)
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

			userMenu.addSubMenu(m->niceName, modelMenu);
		} else
		{
			userMenu.addItem(index, m->niceName);
			mList.add(m);
			index++;
		}
	}

	menu.addSubMenu(LightBlockModelLibrary::getInstance()->genericBlocks.niceName, genericMenu);
	menu.addSubMenu(LightBlockModelLibrary::getInstance()->liveFeedBlocks.niceName, liveFeedMenu);
	menu.addSubMenu(LightBlockModelLibrary::getInstance()->userBlocks.niceName, userMenu);

	return mList;
}
