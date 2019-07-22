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
#include "Prop/Prop.h"

LightBlockClip::LightBlockClip(LightBlockLayer * layer, float _time) :
	LayerBlock("LightBlockClip", _time),
	layer(layer),
	clipNotifier(10)
{
	itemDataType = "LightBlockClip";

	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showProvidersAndGet;

	//autoFade = addBoolParameter("Auto Fade", "If checked, when clips are overlapping, fade will be adjusted automatically", true);

	fadeIn = addFloatParameter("Fade In", "Fade in time", 0, 0, getTotalLength(), false);
	fadeIn->canBeDisabledByUser = true;
	fadeOut = addFloatParameter("Fade Out", "Fade out time", 0, 0, getTotalLength(), false);
	fadeOut->canBeDisabledByUser = true;
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
		result.fill(Colours::transparentBlack);
		return result;
	}
		
	
	float factor = 1;

	double relTimeTotal = absoluteTime - time->floatValue();
	if (fadeIn->floatValue() > 0) factor *= jmin<double>(relTimeTotal / fadeIn->floatValue(),1.f);
	if (fadeOut->floatValue() > 0) factor *= jmin<double>((getTotalLength() - relTimeTotal) / fadeOut->floatValue(), 1.f);
	factor = jmax(factor, 0.f);

	if (dynamic_cast<TimelineBlock *>(currentBlock->provider.get()) != nullptr)
	{
		params.getDynamicObject()->setProperty("sequenceTime", false);
	}


	double relTimeLooped = getRelativeTime(absoluteTime, true);
	Array<Colour> colors = currentBlock->getColors(p, relTimeLooped, params);
	for (int i = 0; i < resolution; i++)
	{
		colors.set(i,colors[i].withMultipliedAlpha(factor));
	}
	return colors;
}

void LightBlockClip::blockParamControlModeChanged(Parameter * p) 
{
	if (p->controlMode == Parameter::AUTOMATION)
	{
		p->automation->setLength(coreLength->floatValue());
		p->automation->setAllowKeysOutside(true);
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
			if (pa->automation == nullptr) continue;
			pa->automation->setLength(coreLength->floatValue(), stretch, stickToCoreEnd);
			pa->automation->setAllowKeysOutside(true);
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
	/*
	else if (p == autoFade)
	{
		fadeIn->setControllableFeedbackOnly(autoFade->boolValue());
		fadeOut->setControllableFeedbackOnly(autoFade->boolValue());
	}*/
}

void LightBlockClip::controllableStateChanged(Controllable* c)
{
	LayerBlock::controllableStateChanged(c);
	if (c == fadeIn || c == fadeOut)
	{
		clipListeners.call(&ClipListener::clipFadesChanged, this);
		clipNotifier.addMessage(new ClipEvent(ClipEvent::FADES_CHANGED, this));
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
			if (pa->automation == nullptr) continue;
			pa->automation->setAllowKeysOutside(true);
		}
	}

	//Retro compatibility, to remove after
	var params = data.getProperty("parameters",var());
	for (int i = 0; i < params.size(); i++)
	{
		if (params[i].getProperty("controlAddress", "") == "/autoFade")
		{
			bool val = params[i].getProperty("value", false);
			fadeIn->setEnabled(!val);
			fadeOut->setEnabled(!val);
			break;
		}
	}
	
	

}
