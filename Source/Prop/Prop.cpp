/*
  ==============================================================================

	Prop.cpp
	Created: 10 Apr 2018 6:59:25pm
	Author:  Ben

  ==============================================================================
*/

#include "Prop.h"
#include "LightBlock/model//LightBlockModelLibrary.h"

Prop::Prop(const String &name, var) :
	BaseItem(name),
	Thread("Prop "+name),
	sendRate(50),
	currentBlock(nullptr),
	hasRealtimeControl(true),
	propNotifier(50)
{
	id = addIntParameter("ID", "Prop ID", 0, 0, 100);
	resolution = addIntParameter("Resolution", "Number of controllable colors in the prop", 1, 1, INT32_MAX);
	colors.resize(resolution->intValue());

	shape = addEnumParameter("Shape", "The shape of the prop");
	shape->addOption("Club", CLUB)->addOption("Ball", BALL)->addOption("Poi", POI)->addOption("Hoop", HOOP);

	activeProvider= addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showProvidersAndGet;

	battery = addFloatParameter("Battery", "The battery level, between 0 and 1", 0);
	battery->setControllableFeedbackOnly(true);

	findPropMode = addBoolParameter("Find Prop", "When active, the prop will lit up 50% white fixed to be able to find it", false);

	bakeStartTime = addFloatParameter("Bake Start Time", "Set the start time of baking", 0,0);
	bakeStartTime->defaultUI = FloatParameter::TIME;
	bakeEndTime = addFloatParameter("Bake End Time", "Set the end time of baking", 1,1);
	bakeEndTime->defaultUI = FloatParameter::TIME;
	bakeFrequency = addIntParameter("Bake Frequency", "The frequency at which to bake", 50, 1, 500);
	bakeAndUploadTrigger = addTrigger("Bake and Upload", "Bake the current assigned block and upload it to the prop");
	
	isBaking = addBoolParameter("Is Baking", "Is this prop currently baking ?", false);
	isBaking->setControllableFeedbackOnly(true);

}

Prop::~Prop()
{
	signalThreadShouldExit();
	waitForThreadToExit(100);
	setBlockFromProvider(nullptr);
}

void Prop::clearItem()
{
	colors.fill(Colours::black);
	sendColorsToProp();
	setBlockFromProvider(nullptr);
}

void Prop::setBlockFromProvider(LightBlockColorProvider * model)
{
	if (currentBlock == nullptr && model == nullptr) return;
	if (model != nullptr && currentBlock != nullptr && currentBlock->provider == model) return;

	if (currentBlock != nullptr)
	{
		signalThreadShouldExit();
		waitForThreadToExit(100);

		removeChildControllableContainer(currentBlock);
		if(!currentBlock->provider.wasObjectDeleted()) currentBlock->provider->removeInspectableListener(this);
		//currentBlock->removeLightBlockListener(this);
		currentBlock = nullptr;
	}

	if(model != nullptr) currentBlock = new LightBlock(model);
	

	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
		currentBlock->provider->addInspectableListener(this);
		startThread();
	}

	propListeners.call(&PropListener::propBlockChanged, this);
	propNotifier.addMessage(new PropEvent(PropEvent::BLOCK_CHANGED, this));
}

void Prop::update()
{
	if (currentBlock != nullptr)
	{
		double time = (Time::getMillisecondCounter() % (int)1e9) / 1000.0;
		colors = currentBlock->getColors(id->intValue(), resolution->intValue(), time, var());

		propListeners.call(&PropListener::colorsUpdated, this);
		propNotifier.addMessage(new PropEvent(PropEvent::COLORS_UPDATED, this));


	}
	if (!findPropMode->boolValue()) sendColorsToProp();
}

void Prop::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	} else if (p == resolution)
	{
		colors.resize(resolution->intValue());
	} else if (p == findPropMode)
	{
		colors.fill(findPropMode->boolValue() ? Colours::white.withBrightness(.2f) : Colours::black);
		sendColorsToProp();
	} else if (p == enabled)
	{
		if (!enabled->boolValue())
		{
			colors.fill(Colours::black);
			sendColorsToProp(true);
		}
	}
}

void Prop::onContainerTriggerTriggered(Trigger * t)
{
	BaseItem::onContainerTriggerTriggered(t);
	if (t == bakeAndUploadTrigger)
	{
		if (currentBlock != nullptr)
		{
			isBaking->setValue(true);
		}else NLOGWARNING(niceName, "Current block not assigned, cannot bake");

	}
}

void Prop::inspectableDestroyed(Inspectable * i)
{
	if (currentBlock != nullptr && i == currentBlock->provider) setBlockFromProvider(nullptr);
}


void Prop::sendColorsToProp(bool forceSend)
{
	if (!enabled->boolValue() && !forceSend) return;
	sendColorsToPropInternal();
}


Array<Prop::TimedColors> Prop::bakeCurrentBlock()
{
	
	Array<TimedColors> result;
	if (currentBlock == nullptr) return result;
	
	NLOG(niceName, "Baking block " << currentBlock->niceName);

	double stepTime = 1.0 / bakeFrequency->intValue();
	float startTime = bakeStartTime->floatValue();
	float endTime = bakeEndTime->floatValue();
	int pId = id->intValue();

	int pResolution = resolution->intValue();
	var params = new DynamicObject();

	params.getDynamicObject()->setProperty("updateAutomation", false); //Avoid actually updating the block parameter automations if there are some (in timeline for instance)

	for (double curTime = startTime; curTime <= endTime; curTime += stepTime)
	{
		result.add({ curTime - startTime, currentBlock->getColors(pId, pResolution, curTime, params) });
	}

	return result;
}


void Prop::uploadCurrentBlock(Array<TimedColors> bakedColors)
{
	NLOG(niceName, "Uploading here... " << bakedColors.size() << " frames");
}

var Prop::getJSONData()
{
	var data = BaseItem::getJSONData();
	if (currentBlock != nullptr) data.getDynamicObject()->setProperty("block", currentBlock->getJSONData());
	if (propId.isNotEmpty()) data.getDynamicObject()->setProperty("propId", propId);
	return data;
}

void Prop::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	if (currentBlock != nullptr) currentBlock->loadJSONData(data.getProperty("block", var()));
	propId = data.getProperty("propId", "");
}

void Prop::run()
{
	while (!threadShouldExit())
	{
		if (isBaking->boolValue())
		{
			Array<TimedColors> bakedColors = bakeCurrentBlock();
			if (bakedColors.size() > 0) uploadCurrentBlock(bakedColors);
			isBaking->setValue(false);
		}
		else
		{
			update();
			sleep(1000.0f / sendRate); //60fps
		}
	}
}
