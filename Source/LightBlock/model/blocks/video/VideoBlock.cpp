/*
  ==============================================================================

	VideoBlock.cpp
	Created: 10 Apr 2018 6:58:49pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"

VideoBlock::VideoBlock(const String &name, var params) :
	LightBlockModel(name, params)
{

	Spatializer::getInstance()->addBaseManagerListener(this);

	inputIsLive = addBoolParameter("Input is Live", "If there is something coming, you'll know it here", false);
	inputIsLive->setControllableFeedbackOnly(true);

	currentLayout = paramsContainer->addEnumParameter("Layout", "The Layout to use. You can edit them by clicking inside the Spatializer window");

	saveAndLoadName = false;
	saveType = false;


	updateLayoutOptions();
}

VideoBlock::~VideoBlock()
{
	Spatializer::getInstance()->removeBaseManagerListener(this);
}

void VideoBlock::clear()
{
	LightBlockModel::clear();
}

void VideoBlock::setSelectedInternal(bool value)
{
	LightBlockModel::setSelectedInternal(value);

	if (Engine::mainEngine->isLoadingFile || Engine::mainEngine->isClearing) return;
	if (ShapeShifterManager::getInstanceWithoutCreating() == nullptr) return;
	if (SpatializerPanel* p = ShapeShifterManager::getInstance()->getContentForType<SpatializerPanel>())
	{
		if (value) p->setVideoBlock(this);
		//else if (p->VideoBlock == this) p->setVideoBlock(nullptr);
	}
}

void VideoBlock::updateLayoutOptions()
{
	String layoutName = currentLayout->getValueData().toString();
	currentLayout->clearOptions();
	for (auto& layout : Spatializer::getInstance()->items)
	{
		currentLayout->addOption(layout->niceName, layout->shortName, false);
	}

	currentLayout->setValueWithData(layoutName);
}

Array<Colour> VideoBlock::getColors(Prop* p, double time, var params)
{
	if (!inputIsLive->boolValue()) return LightBlockModel::getColors(p, time, params);
	SpatLayout* layout = Spatializer::getInstance()->getItemWithName(getParamValue<var>(currentLayout, params).toString());
	
	Array<Colour> result = Spatializer::getInstance()->getColors(getImage(), p, layout);//

	if(result.isEmpty()) return LightBlockModel::getColors(p, time, params);
	return result;
}

void VideoBlock::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	LightBlockModel::onControllableFeedbackUpdateInternal(cc, c);

	if (c == currentLayout)
	{
		Spatializer::getInstance()->setCurrentLayout(Spatializer::getInstance()->getItemWithName(currentLayout->getValueData().toString()));
	}
}

void VideoBlock::itemAdded(SpatLayout*)
{
	updateLayoutOptions();
	if (Spatializer::getInstance()->items.size() == 1) currentLayout->setValueWithData(Spatializer::getInstance()->items[0]->shortName);
}

void VideoBlock::itemsAdded(Array<SpatLayout*>)
{
	updateLayoutOptions();
}

void VideoBlock::itemRemoved(SpatLayout*)
{
	updateLayoutOptions();
}

void VideoBlock::itemsRemoved(Array<SpatLayout*>)
{
	updateLayoutOptions();

}
