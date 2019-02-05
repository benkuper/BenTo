/*
  ==============================================================================

	Prop.cpp
	Created: 10 Apr 2018 6:59:25pm
	Author:  Ben

  ==============================================================================
*/

#include "Prop.h"
#include "LightBlock/model//LightBlockModelLibrary.h"
#include "PropManager.h"

Prop::Prop(StringRef name, StringRef familyName, var) :
	BaseItem(name),
	Thread("Prop " + name),
	family(nullptr),
	generalCC("General"),
	ioCC("Communication"),
	sensorsCC("SensorsCC"),
	bakingCC("Bake and Upload"),
	currentBlock(nullptr),
	previousID(-1),
	updateRate(30),
	propNotifier(50)
{
	registerFamily(familyName);
	
	saveAndLoadRecursiveData = true;

	addChildControllableContainer(&generalCC);
	globalID = generalCC.addIntParameter("Global ID", "The Global Prop ID, it is a unique ID but it can be swapped between props", 0, 0, 100);
	resolution = generalCC.addIntParameter("Resolution", "Number of controllable colors in the prop", 1, 1, INT32_MAX);
	type = generalCC.addEnumParameter("Type", "The type of the prop");
	fillTypeOptions(type);

	colors.resize(resolution->intValue());

	addChildControllableContainer(&ioCC);
	ioCC.editorIsCollapsed = true;
	findPropMode = addBoolParameter("Find Prop", "When active, the prop will lit up 50% white fixed to be able to find it", false);
	findPropMode->setControllableFeedbackOnly(true);
	findPropMode->isSavable = false;
	
	addChildControllableContainer(&sensorsCC);
	battery = sensorsCC.addFloatParameter("Battery", "The battery level, between 0 and 1", 0);
	battery->setControllableFeedbackOnly(true);

	addChildControllableContainer(&bakingCC);
	bakingCC.editorIsCollapsed = true;
	bakeStartTime = bakingCC.addFloatParameter("Bake Start Time", "Set the start time of baking", 0,0);
	bakeStartTime->defaultUI = FloatParameter::TIME;
	bakeEndTime = bakingCC.addFloatParameter("Bake End Time", "Set the end time of baking", 1,1);
	bakeEndTime->defaultUI = FloatParameter::TIME;
	bakeFrequency = bakingCC.addIntParameter("Bake Frequency", "The frequency at which to bake", 50, 1, 500);
	bakeAndUploadTrigger = bakingCC.addTrigger("Bake and Upload", "Bake the current assigned block and upload it to the prop");
	
	isBaking = bakingCC.addBoolParameter("Is Baking", "Is this prop currently baking ?", false);
	isBaking->hideInEditor = true;
	isBaking->setControllableFeedbackOnly(true);

	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showProvidersAndGet;
	activeProvider->hideInEditor = true;

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
	if (family != nullptr) family->unregisterProp(this);
}

void Prop::registerFamily(StringRef familyName)
{
	PropFamily * f = PropManager::getInstance()->getFamilyWithName(familyName);
	if (f != nullptr)
	{
		family = f;
		family->registerProp(this);
	}
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

	if (Engine::mainEngine != nullptr && !Engine::mainEngine->isClearing)
	{
		propListeners.call(&PropListener::propBlockChanged, this);
		propNotifier.addMessage(new PropEvent(PropEvent::BLOCK_CHANGED, this));
	}
	
}

void Prop::update()
{
	if (findPropMode->boolValue())
	{
		colors.fill(Colours::white.withBrightness(.3f));
	}else if (currentBlock != nullptr)
	{
		double time = (Time::getMillisecondCounter() % (int)1e9) / 1000.0;
		colors = currentBlock->getColors(this, time, var());

		if (Engine::mainEngine != nullptr && !Engine::mainEngine->isClearing)
		{
			propListeners.call(&PropListener::colorsUpdated, this);
			propNotifier.addMessage(new PropEvent(PropEvent::COLORS_UPDATED, this));
		}
	}

	sendColorsToProp();
}

void Prop::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	}else if (p == enabled)
	{
		if (!enabled->boolValue())
		{
			colors.fill(Colours::black);
			sendColorsToProp(true);
		}
	}
}

void Prop::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	if (c == resolution)
	{
		colors.resize(resolution->intValue());
	}
	else if (c == globalID)
	{
		propListeners.call(&PropListener::propIDChanged, this, previousID);
		previousID = globalID->intValue();
	}
	else if (c == findPropMode)
	{
		if(!findPropMode->boolValue()) colors.fill(Colours::black);
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

void Prop::fillTypeOptions(EnumParameter * p)
{
	p->addOption("Club", CLUB)->addOption("Ball", BALL)->addOption("Poi", POI)->addOption("Hoop", HOOP)->addOption("Ring", RING)->addOption("Buggeng", BUGGENG)->addOption("Box",BOX);
}

Array<Prop::TimedColors> Prop::bakeCurrentBlock()
{
	Array<TimedColors> result;
	if (currentBlock == nullptr) return result;
	
	NLOG(niceName, "Baking block " << currentBlock->niceName);

	double stepTime = 1.0 / bakeFrequency->intValue();
	float startTime = bakeStartTime->floatValue();
	float endTime = bakeEndTime->floatValue();

	var params = new DynamicObject();

	params.getDynamicObject()->setProperty("updateAutomation", false); //Avoid actually updating the block parameter automations if there are some (in timeline for instance)

	for (double curTime = startTime; curTime <= endTime; curTime += stepTime)
	{
		result.add({ curTime - startTime, currentBlock->getColors(this, curTime, params) });
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
	if (deviceID.isNotEmpty()) data.getDynamicObject()->setProperty("deviceID", deviceID);
	return data;
}

void Prop::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	if (currentBlock != nullptr) currentBlock->loadJSONData(data.getProperty("block", var()));
	deviceID = data.getProperty("deviceID", "");
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
			sleep(1000.0f / updateRate); //60fps
		}
	}
}
