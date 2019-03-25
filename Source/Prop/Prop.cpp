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
	updateRate(50),
	propNotifier(50)
{
	registerFamily(familyName);
	
	editorIsCollapsed = true;

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
	bakeFrequency = bakingCC.addIntParameter("Bake Frequency", "The frequency at which to bake", 100, 1, 800);
	bakeAndUploadTrigger = bakingCC.addTrigger("Bake and Upload", "Bake the current assigned block and upload it to the prop");
	bakeAndExportTrigger = bakingCC.addTrigger("Bake and Export", "Bake the current assigned block and export it to a file");
	bakeFileName = bakingCC.addStringParameter("Bake file name", "Name of the bake file to send and to play", "demo.colors");
	bakeMode = bakingCC.addBoolParameter("Bake Mode", "Play the bake file with name set above, or revert to streaming", false);	
	
	sendCompressedFile = bakingCC.addBoolParameter("Send Compressed File", "Send Compressed File instead of raw", false);

	isBaking = bakingCC.addBoolParameter("Is Baking", "Is this prop currently baking ?", false);
	isBaking->hideInEditor = true;
	isBaking->setControllableFeedbackOnly(true);
	isBaking->isSavable = false;

	isUploading = bakingCC.addBoolParameter("Is Uploading", "Upload after bake", false);
	isUploading->hideInEditor = true;
	isUploading->setControllableFeedbackOnly(true);
	isUploading->isSavable = false;

	bakingProgress = bakingCC.addFloatParameter("Baking progress", "", 0, 0, 1);
	bakingProgress->setControllableFeedbackOnly(true);
	uploadProgress = bakingCC.addFloatParameter("Upload progress", "", 0, 0, 1);
	uploadProgress->setControllableFeedbackOnly(true);

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
		if (!currentBlock->provider.wasObjectDeleted())
		{
			currentBlock->provider->setHighlighted(false);
			
			linkedInspectables.removeAllInstancesOf(currentBlock->provider.get());
			currentBlock->provider->linkedInspectables.removeAllInstancesOf(this);
			currentBlock->provider->removeColorProviderListener(this);
			currentBlock->provider->removeInspectableListener(this);
		}

		//currentBlock->removeLightBlockListener(this);
		currentBlock = nullptr;
	}

	if(model != nullptr) currentBlock = new LightBlock(model);
	

	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
		currentBlock->provider->addInspectableListener(this);
		currentBlock->provider->addColorProviderListener(this);

		linkedInspectables.addIfNotAlreadyThere(currentBlock->provider.get());
		currentBlock->provider->linkedInspectables.addIfNotAlreadyThere(this);

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

	if(!bakeMode->boolValue()) sendColorsToProp();
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
	}else if(c == bakeAndUploadTrigger || c == bakeAndExportTrigger)
	{
		if (currentBlock != nullptr)
		{
			afterBake = c == bakeAndUploadTrigger ? UPLOAD : EXPORT;

			if (afterBake == EXPORT)
			{
				FileChooser fc("Export a block");
				if(fc.browseForFileToSave(true)) exportFile = fc.getResult();
				else return;
			}

			bakingProgress->setValue(0);
			uploadProgress->setValue(0);
			isBaking->setValue(true);
		}
		else
		{
			NLOGWARNING(niceName, "Current block not assigned, cannot bake");
		}
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

Prop::BakeData Prop::bakeCurrentBlock()
{
	BakeData result;
	if (currentBlock == nullptr) return result;
	
	NLOG(niceName, "Baking block " << currentBlock->niceName);

	result.name = bakeFileName->stringValue();// currentBlock->shortName + "_" + globalID->stringValue();
	result.fps = bakeFrequency->intValue();
	result.numFrames = 0;
	result.metaData = var(new DynamicObject());
	result.metaData.getDynamicObject()->setProperty("block", currentBlock->niceName);
	result.metaData.getDynamicObject()->setProperty("id", globalID->intValue());

	double stepTime = 1.0 / bakeFrequency->intValue();
	float startTime = bakeStartTime->floatValue();
	float endTime = bakeEndTime->floatValue();

	var params = new DynamicObject();

	params.getDynamicObject()->setProperty("updateAutomation", false); //Avoid actually updating the block parameter automations if there are some (in timeline for instance)
	params.getDynamicObject()->setProperty("sequenceTime", false); //Avoid using sequence's time

	
	MemoryOutputStream os(result.data, false);
	for (double curTime = startTime; curTime <= endTime; curTime += stepTime)
	{

		Array<Colour> cols = currentBlock->getColors(this, curTime, params);
		for (auto &c : cols) os.writeInt(c.getARGB()); 
		result.numFrames++;

		DBG("Check for cur time " << curTime << " / " << cols[0].toString());
		bakingProgress->setValue(jmap<float>(curTime, startTime, endTime, 0, 1));
	}

	os.flush();

	return result;
}


void Prop::uploadBakedData(BakeData bakedColors)
{
	NLOG(niceName, "Uploading here... " << bakedColors.numFrames << " frames");
}

void Prop::exportBakedData(BakeData data)
{
	NLOG(niceName, "Export bake data " << data.name);
}

void Prop::providerBakeControlUpdate(LightBlockColorProvider::BakeControl control, var data)
{
	if (!bakeMode->boolValue()) return;

	switch (control)
	{
	case LightBlockColorProvider::BakeControl::PLAY:
		playBake((float)data);
		break;

	case LightBlockColorProvider::BakeControl::PAUSE:
		pauseBakePlaying();
		break;

	case LightBlockColorProvider::BakeControl::SEEK:
		seekBakePlaying((float)data);
		break;

	case LightBlockColorProvider::BakeControl::STOP:
		stopBakePlaying();
		break;
	}
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
	sleep(100);

	while (!threadShouldExit())
	{
		if (isBaking->boolValue())
		{
			BakeData data = bakeCurrentBlock();

			switch (afterBake)
			{
			case UPLOAD:
				isUploading->setValue(true);
				uploadBakedData(data);
				isUploading->setValue(false);
				break;

			case EXPORT:
				exportBakedData(data);
				break;
                    
                default:
                    break;
			}

			isBaking->setValue(false);
		}
		else
		{
			update();
			sleep(1000.0f / updateRate); //50fps
		}
	}
}
