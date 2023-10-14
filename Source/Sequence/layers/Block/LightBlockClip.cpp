/*
  ==============================================================================

    LightBlockClip.cpp
    Created: 17 Apr 2018 5:10:24pm
    Author:  Ben

  ==============================================================================
*/

#include "Sequence/SequenceIncludes.h"

LightBlockClip::LightBlockClip(LightBlockLayer * layer) :
	LayerBlock(getTypeString()),
	effects("Block Effects"),
	layer(layer),
	settingLengthFromMethod(false),
	clipNotifier(10)
{
	itemDataType = "LightBlockClip";

	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showSourcesAndGet;

	fadeIn = addFloatParameter("Fade In", "Fade in time", 0, 0, getTotalLength(), false);
	fadeIn->canBeDisabledByUser = true;
	fadeOut = addFloatParameter("Fade Out", "Fade out time", 0, 0, getTotalLength(), false);
	fadeOut->canBeDisabledByUser = true;

	timeOffsetByID = addFloatParameter("Time Offset by ID", "Offset computed time by Prop ID", 0);
	timeOffsetByID->defaultUI = FloatParameter::TIME;


	effects.userCanAddItemsManually = false;
	addChildControllableContainer(&effects);

	filterManager.reset(new PropTargetFilterManager(&((SequenceBlockSequence*)layer->sequence)->clusterGroupManager));
	addChildControllableContainer(filterManager.get());
}

LightBlockClip::~LightBlockClip()
{
	setBlockFromProvider(nullptr);
}

void LightBlockClip::setBlockFromProvider(LightBlockColorProvider * provider)
{
	if (currentBlock == nullptr && provider == nullptr) return;

	if (provider != nullptr && currentBlock != nullptr && currentBlock->provider == provider) return;

	var prevData;
	if (currentBlock != nullptr)
	{
		prevData = currentBlock->getJSONData();
		removeChildControllableContainer(currentBlock.get());
		currentBlock->removeLightBlockListener(this);
		currentBlock = nullptr;
	}

	if (provider != nullptr) currentBlock.reset(new LightBlock(provider));


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock.get(), false, 0);
		currentBlock->addLightBlockListener(this);
		currentBlock->loadJSONData(prevData);

		LightBlockModelPreset* preset = dynamic_cast<LightBlockModelPreset*>(provider);
		if (preset == nullptr)
		{
			Array<WeakReference<Parameter>> blockParams = currentBlock->getAllParameters(true);
			for (auto& p : blockParams) p->isOverriden = true;
		}
	}
}
Array<Colour> LightBlockClip::getColors(Prop * p, double absoluteTime, var params)
{

	int resolution = p->getResolution();

	if (currentBlock == nullptr)
	{
		Array<Colour> result;
		result.resize(resolution);
		result.fill(Colours::transparentBlack);
		return result;
	}
		
	
	float factor = 1;

	int id = params.getProperty("forceID", p->globalID->intValue());
	float offset = id * timeOffsetByID->floatValue();
	double relTimeTotal = absoluteTime - time->floatValue() + offset;
	if (fadeIn->floatValue() > 0) factor *= jmin<double>(relTimeTotal / fadeIn->floatValue(),1.f);
	if (fadeOut->floatValue() > 0) factor *= jmin<double>((getTotalLength() - relTimeTotal) / fadeOut->floatValue(), 1.f);
	factor = jmax(factor, 0.f);

	if (dynamic_cast<SequenceBlock *>(currentBlock->provider.get()) != nullptr)
	{
		params.getDynamicObject()->setProperty("sequenceTime", false);
	}


	double relTimeLooped = getRelativeTime(absoluteTime, true);
	relTimeLooped = fmodf(relTimeLooped + offset, getCoreEndTime());
	Array<Colour> colors = currentBlock->getColors(p, relTimeLooped, params);

	for (int i = 0; i < effects.items.size(); i++)
	{
		if (!effects.items[i]->enabled->boolValue()) continue;
		if (effects.items[i]->lightBlock == nullptr) continue;
		effects.items[i]->lightBlock->filterColors(&colors, p, relTimeLooped, params);
	}

	for (int i = 0; i < resolution; i++)
	{
		colors.set(i,colors[i].withMultipliedAlpha(factor));
	}

	return colors;
}

void LightBlockClip::addEffectFromProvider(LightBlockColorProvider * provider)
{
	LightBlockEffect * e = effects.addItem();
	e->setProvider(provider);

	notifyUpdatePreview();

}

void LightBlockClip::blockParamControlModeChanged(Parameter * p) 
{
	if (p->controlMode == Parameter::AUTOMATION)
	{
		p->automation->setAllowKeysOutside(true);
		p->automation->setLength(coreLength->floatValue(),true);
	}
}

void LightBlockClip::setCoreLength(float value, bool stretch, bool stickToCoreEnd)
{
	settingLengthFromMethod = true;
	LayerBlock::setCoreLength(value, stretch, stickToCoreEnd);

	if (currentBlock != nullptr)
	{
		Array<WeakReference<Parameter>> params = currentBlock->paramsContainer.getAllParameters();
		for (auto & pa : params)
		{
			if (pa->automation == nullptr) continue;
			pa->automation->setAllowKeysOutside(true);
			pa->automation->setLength(coreLength->floatValue(), stretch, stickToCoreEnd);
		}
	}
	settingLengthFromMethod = false;
}

void LightBlockClip::notifyUpdatePreview()
{
	clipNotifier.addMessage(new ClipEvent(ClipEvent::REGENERATE_PREVIEW, this));

}

void LightBlockClip::handleEnterExit(bool enter, Array<Prop*> filteredProps)
{
	if(currentBlock == nullptr) return;
	if (enter == isActive->boolValue()) return;
	isActive->setValue(enter);

	if(filterManager->items.isEmpty()) currentBlock->handleEnterExit(isActive->boolValue(), filteredProps);
	else
	{
		Array<Prop*> clipProps;
		for (auto& p : filteredProps) if (filterManager->getTargetIDForProp(p) >= 0) clipProps.add(p);
		currentBlock->handleEnterExit(isActive->boolValue(), clipProps);
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

		if (p == coreLength && !settingLengthFromMethod)
		{
			setCoreLength(coreLength->floatValue());
		}
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
	data.getDynamicObject()->setProperty("filters", filterManager->getJSONData());
	data.getDynamicObject()->setProperty("effects", effects.getJSONData());
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

	filterManager->loadJSONData(data.getProperty("filters", var()));

	//Effects need a way to provide the LightBlockProvider when creating lightblock, otherwise crashes
	effects.loadJSONData(data.getProperty("effects", var()));

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
