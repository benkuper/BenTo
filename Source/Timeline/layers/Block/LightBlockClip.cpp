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
	LayerBlock("LightBlockClip", _time),
	layer(layer)
{
	itemDataType = "LightBlockClip";

	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showProvidersAndGet;

	autoFade = addBoolParameter("Auto Fade", "If checked, when clips are overlapping, fade will be adjusted automatically", true);
	fadeIn = addFloatParameter("Fade In", "Fade in time", 0, 0, getTotalLength());
	fadeOut = addFloatParameter("Fade Out", "Fade out time", 0, 0, getTotalLength());
	fadeIn->setControllableFeedbackOnly(autoFade->boolValue());
	fadeOut->setControllableFeedbackOnly(autoFade->boolValue());
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
		removeChildControllableContainer(currentBlock.get());
		currentBlock->removeLightBlockListener(this);
		currentBlock = nullptr;
	}

	if (provider != nullptr) currentBlock.reset(new LightBlock(provider));


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock.get());
		currentBlock->addLightBlockListener(this);

	}
}
Array<Colour> LightBlockClip::getColors(Prop * p, double absoluteTime, var params)
{

	int resolution = p->resolution->intValue();

	if (currentBlock == nullptr)
	{
		Array<Colour> result;
		result.resize(resolution);
		result.fill(Colours::black);
		return result;
	}
		
	
	float factor = 1;

	double relTimeTotal = absoluteTime - time->floatValue();
	if (fadeIn->floatValue() > 0) factor *= jmin<double>(relTimeTotal / fadeIn->floatValue(),1.f);
	if (fadeOut->floatValue() > 0) factor *= jmin<double>((getTotalLength() - relTimeTotal) / fadeOut->floatValue(), 1.f);

	if (dynamic_cast<TimelineBlock *>(currentBlock->provider.get()) != nullptr)
	{
		params.getDynamicObject()->setProperty("sequenceTime", false);
	}

	double relTimeLooped = getRelativeTime(absoluteTime, true);
	Array<Colour> colors = currentBlock->getColors(p, relTimeLooped, params);
	for (int i = 0; i < resolution; i++)
	{
		colors.set(i,colors[i].withMultipliedBrightness(factor));
	}
	return colors;
}

void LightBlockClip::blockParamControlModeChanged(Parameter * p) 
{
	if (p->controlMode == Parameter::AUTOMATION)
	{
		p->automation->automation.setLength(coreLength->floatValue());
		p->automation->automation.allowKeysOutside = true;
	}
}

void LightBlockClip::setCoreLength(float value, bool stretch, bool stickToCoreEnd)
{
	LayerBlock::setCoreLength(value, stretch, stickToCoreEnd);

	if (currentBlock != nullptr)
	{
		Array<WeakReference<Parameter>> params = currentBlock->paramsContainer.getAllParameters();
		for (auto & pa : params)
		{
			if (pa->controlMode == Parameter::AUTOMATION) pa->automation->automation.setLength(coreLength->floatValue(), stretch, stickToCoreEnd);
		}
	}
}

void LightBlockClip::onContainerParameterChangedInternal(Parameter * p)
{
	LayerBlock::onContainerParameterChangedInternal(p);

	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	}
	else if (p == coreLength || p == loopLength)
	{
		fadeIn->setRange(0, getTotalLength());
		fadeOut->setRange(0, getTotalLength());
	}
	else if (p == autoFade)
	{
		fadeIn->setControllableFeedbackOnly(autoFade->boolValue());
		fadeOut->setControllableFeedbackOnly(autoFade->boolValue());
	}
}

var LightBlockClip::getJSONData()
{
	var data = LayerBlock::getJSONData();
	if (currentBlock != nullptr) data.getDynamicObject()->setProperty("blockData", currentBlock->getJSONData());
	return data;
}

void LightBlockClip::loadJSONDataInternal(var data)
{
	LayerBlock::loadJSONDataInternal(data);
	if (currentBlock != nullptr)
	{
		currentBlock->loadJSONData(data.getProperty("blockData", var()));

		Array<WeakReference<Parameter>> params = currentBlock->paramsContainer.getAllParameters();

		for (auto & pa : params)
		{
			if (pa->controlMode == Parameter::AUTOMATION)
			{
				pa->automation->automation.allowKeysOutside = true;
			}
		}
	}

}
