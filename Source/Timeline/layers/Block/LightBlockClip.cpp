/*
  ==============================================================================

    LightBlockClip.cpp
    Created: 17 Apr 2018 5:10:24pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClip.h"
#include "LightBlock/model/LightBlockModelLibrary.h"

LightBlockClip::LightBlockClip(float _time) :
	BaseItem("LightBlockClip")
{
	itemDataType = "LightBlockClip";

	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showProvidersAndGet;

	startTime = addFloatParameter("Start Time", "Start time", 0, 0, INT32_MAX);
	startTime->setValue(_time);
	startTime->defaultUI = FloatParameter::TIME;
	
	length = addFloatParameter("Length", "Length of the clip (in seconds)", 10, .1f, INT32_MAX);
	length->defaultUI = FloatParameter::TIME;

	clipPosition = addFloatParameter("Position", "Clip position", .5f, 0, 1);
	clipSize = addFloatParameter("Size", "Clip size", .5f, 0, 1);
}

LightBlockClip::~LightBlockClip()
{
}

void LightBlockClip::setBlockFromProvider(LightBlockColorProvider * provider)
{
	if (currentBlock == nullptr && provider == nullptr) return;
	if (provider != nullptr && currentBlock != nullptr && currentBlock->provider == provider) return;

	if (currentBlock != nullptr)
	{
		removeChildControllableContainer(currentBlock);
		currentBlock = nullptr;
	}

	if (provider != nullptr) currentBlock = new LightBlock(provider);


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
	}
}
Array<Colour> LightBlockClip::getColors(int id, int resolution, float time, var params)
{
	if (currentBlock == nullptr)
	{
		Array<Colour> result;
		result.resize(resolution);
		result.fill(Colours::black);
		return result;
	}
		
	return currentBlock->getColors(id, resolution, time, params);

	
}

float LightBlockClip::getTimeForRelativePosition(float t, bool absoluteTime)
{
	return (absoluteTime?startTime->floatValue():0) + length->floatValue()*t; //to improve with speed animation handling
}

bool LightBlockClip::isInRange(float _time)
{
	return (_time >= startTime->floatValue() && _time <= startTime->floatValue() + length->floatValue());

}

void LightBlockClip::onContainerParameterChanged(Parameter * p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	}
}
