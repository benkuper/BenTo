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

	time = addFloatParameter("Time", "Start time", 0, 0, INT_MAX);
	time->setValue(time);
	time->defaultUI = FloatParameter::TIME;
	
	length = addFloatParameter("Length", "Length of the clip (in seconds)", 10, .1f, INT_MAX);
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

	if (provider != nullptr) currentBlock = new LightBlock(provider, &fakeProp);


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
	}
}
Array<Colour> LightBlockClip::getColorsForProp(Prop * p)
{
	//int numLeds = clipSize->floatValue()*p->resolution->intValue();
	//int firstLed = (clipPosition->floatValue() - clipSize->floatValue() / 2) *p->resolution->intValue();
	fakeProp.resolution->setValue(p->resolution->intValue());
	fakeProp.id->setValue(p->id->value);
	if (currentBlock != nullptr) currentBlock->update();

	/*
	Array<Colour> result;
	for (int i = 0; i < firstLed; i++) result.add(Colours::black);
	result.addArray(fakeProp.colors);
	while (result.size() < p->resolution->intValue()) result.add(Colours::black);

	return result;
	*/

	return fakeProp.colors;
}

bool LightBlockClip::isInRange(float _time)
{
	return (_time >= time->floatValue() && _time <= time->floatValue() + length->floatValue());

}

void LightBlockClip::onContainerParameterChanged(Parameter * p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	}
}
