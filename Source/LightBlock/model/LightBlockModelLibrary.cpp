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

LightBlockModel * LightBlockModelLibrary::showAllModelsAndGet()
{
	PopupMenu genericMenu;
	PopupMenu liveFeedMenu;
	PopupMenu userMenu;

	Array<LightBlockModel *> mList;

	int index = 1;
	for (auto &cc : LightBlockModelLibrary::getInstance()->genericBlocks.controllableContainers)
	{
		LightBlockModel * m = dynamic_cast<LightBlockModel *>(cc.get());
		if (m == nullptr) continue;
		mList.add(m);
		genericMenu.addItem(index, m->niceName);
		index++;
	}

	for (auto &cc : LightBlockModelLibrary::getInstance()->liveFeedBlocks.controllableContainers)
	{
		LightBlockModel * m = dynamic_cast<LightBlockModel *>(cc.get());
		if (m == nullptr) continue;
		mList.add(m);
		liveFeedMenu.addItem(index, m->niceName);
		index++;
	}

	for (auto & m: LightBlockModelLibrary::getInstance()->userBlocks.items)
	{
		mList.add(m);
		userMenu.addItem(index, m->niceName);
		index++;
	}
	
	PopupMenu menu;
	menu.addSubMenu(LightBlockModelLibrary::getInstance()->genericBlocks.niceName, genericMenu);
	menu.addSubMenu(LightBlockModelLibrary::getInstance()->liveFeedBlocks.niceName, liveFeedMenu);
	menu.addSubMenu(LightBlockModelLibrary::getInstance()->userBlocks.niceName, userMenu);

	int result = menu.show();

	if (result > 0) return mList[result - 1];
	return nullptr;
}
