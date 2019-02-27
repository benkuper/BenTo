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

	fadeIn = addFloatParameter("Fade In", "Fade in time", 0, 0, getTotalLength());
	fadeOut = addFloatParameter("Fade Out", "Fade out time", 0, 0, getTotalLength());
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
Array<Colour> LightBlockClip::getColors(Prop * p, double absoluteTime, var params)
{
	double relTime = getRelativeTime(absoluteTime, true);

	int resolution = p->resolution->intValue();

	if (currentBlock == nullptr)
	{
		Array<Colour> result;
		result.resize(resolution);
		result.fill(Colours::black);
		return result;
	}
		
	
	float factor = 1;
	if (fadeIn->floatValue() > 0) factor *= jmin<double>(relTime / fadeIn->floatValue(),1.f);
	if (fadeOut->floatValue() > 0) factor *= jmin<double>((getTotalLength() - relTime) / fadeOut->floatValue(), 1.f);


	if (dynamic_cast<TimelineBlock *>(currentBlock->provider.get()) != nullptr)
	{
		params.getDynamicObject()->setProperty("sequenceTime", false);
	}

	Array<Colour> colors = currentBlock->getColors(p, relTime, params);
	for (int i = 0; i < resolution; i++)
	{
		colors.set(i,colors[i].withMultipliedBrightness(factor));
	}
	return colors;
}
void LightBlockClip::onContainerParameterChanged(Parameter * p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	} else if (p == coreLength || p == loopLength)
	{
		fadeIn->setRange(0, getTotalLength());
		fadeOut->setRange(0, getTotalLength());

;		if (currentBlock != nullptr)
		{
			Array<WeakReference<Parameter>> params = currentBlock->paramsContainer.getAllParameters();
			for (auto & pa : params)
			{
				if (p->controlMode == Parameter::AUTOMATION) pa->automation->automation.length->setValue(coreLength->floatValue());
			}
		}
	}
}

void LightBlockClip::blockParamControlModeChanged(Parameter * p) 
{
	if (p->controlMode == Parameter::AUTOMATION) p->automation->automation.length->setValue(coreLength->floatValue());
}

/*
void LightBlockClip::itemAdded(ParameterAutomation * p)
{
	for (auto & a : currentBlock->automationsManager.items) a->automation.length->setValue(length->floatValue());
}
*/

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
	}

}
