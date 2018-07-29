/*
  ==============================================================================

    LightBlockClip.cpp
    Created: 17 Apr 2018 5:10:24pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClip.h"
#include "LightBlock/model/LightBlockModelLibrary.h"
#include "LightBlock/model/blocks/timeline/TimelineBlock.h"

LightBlockClip::LightBlockClip(LightBlockLayer * layer, float _time) :
	BaseItem("LightBlockClip"),
	layer(layer)
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


	fadeIn = addFloatParameter("Fade In", "Fade in time", 0, 0, length->floatValue());
	fadeOut = addFloatParameter("Fade Out", "Fade out time", 0, 0, length->floatValue());
}

LightBlockClip::~LightBlockClip()
{
	setBlockFromProvider(nullptr);
}

void LightBlockClip::setBlockFromProvider(LightBlockColorProvider * provider)
{
	if (currentBlock == nullptr && provider == nullptr) return;

	if (provider != nullptr && currentBlock != nullptr && currentBlock->provider == provider) return;

	if (currentBlock != nullptr)
	{
		removeChildControllableContainer(currentBlock);
		currentBlock->removeLightBlockListener(this);
		//currentBlock->automationsManager.removeBaseManagerListener(this);
		currentBlock = nullptr;
	}

	if (provider != nullptr) currentBlock = new LightBlock(provider);


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
		currentBlock->addLightBlockListener(this);
		//currentBlock->automationsManager.addBaseManagerListener(this);
		//for (auto & a : currentBlock->automationsManager.items) a->automation.length->setValue(length->floatValue());

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
		
	
	float factor = 1;
	if (fadeIn->floatValue() > 0) factor *= jmin(time / fadeIn->floatValue(),1.f);
	if (fadeOut->floatValue() > 0) factor *= jmin((length->floatValue() - time) / fadeOut->floatValue(), 1.f);


	if (dynamic_cast<TimelineBlock *>(currentBlock->provider.get()) != nullptr)
	{
		params.getDynamicObject()->setProperty("sequenceTime", false);
	}

	Array<Colour> colors = currentBlock->getColors(id, resolution, time, params);
	for (int i = 0; i < resolution; i++)
	{
		colors.set(i,colors[i].withMultipliedBrightness(factor));
	}
	return colors;
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
	} else if (p == length)
	{
		fadeIn->setRange(0, length->floatValue());
		fadeOut->setRange(0, length->floatValue());

;		if (currentBlock != nullptr)
		{
			Array<WeakReference<Parameter>> params = currentBlock->paramsContainer.getAllParameters();
			for (auto & pa : params)
			{
				if (p->controlMode == Parameter::AUTOMATION) pa->automation->automation.length->setValue(length->floatValue());
			}
		}
	}
}

void LightBlockClip::blockParamControlModeChanged(Parameter * p) 
{
	if (p->controlMode == Parameter::AUTOMATION) p->automation->automation.length->setValue(length->floatValue());
}

/*
void LightBlockClip::itemAdded(ParameterAutomation * p)
{
	for (auto & a : currentBlock->automationsManager.items) a->automation.length->setValue(length->floatValue());
}
*/

var LightBlockClip::getJSONData()
{
	var data = BaseItem::getJSONData();
	if (currentBlock != nullptr) data.getDynamicObject()->setProperty("blockData", currentBlock->getJSONData());
	return data;
}

void LightBlockClip::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	if (currentBlock != nullptr)
	{
		currentBlock->loadJSONData(data.getProperty("blockData", var()));
	}

}
