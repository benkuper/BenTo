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

Prop::Prop(var params) :
	BaseItem(params.getProperty("name","Unknown").toString()),
	Thread("Prop"),
	family(nullptr),
	generalCC("General"),
	connectionCC("Connection"),
	controlsCC("Controls"),
	sensorsCC("Sensors"),
	bakingCC("Bake and Upload"),
	receivedPongSinceLastPingSent(false),
	providerToBake(nullptr),
	currentBlock(nullptr),
	previousID(-1),
	updateRate(50),
	propNotifier(50)
{
	registerFamily(params.getProperty("family","Mistery Family").toString());

	customType = params.getProperty("type", "");

	showWarningInUI = true;
	editorIsCollapsed = true;
	saveAndLoadRecursiveData = true;

	addChildControllableContainer(&generalCC);
	globalID = generalCC.addIntParameter("Global ID", "The Global Prop ID, it is a unique ID but it can be swapped between props", 0, 0, 100);
	controllableFeedbackMap.set(globalID, "/prop/id");

	resolution = generalCC.addIntParameter("Resolution", "Number of controllable colors in the prop", params.getProperty("resolution",1), 1);
	type = generalCC.addEnumParameter("Type", "The type of the prop");
	fillTypeOptions(type);

	colors.resize(resolution->intValue());

	isConnected = connectionCC.addBoolParameter("Is Connected", "This is checked if the prop is connected.", false);
	isConnected->setControllableFeedbackOnly(true);
	isConnected->isSavable = false; 

	findPropMode = connectionCC.addBoolParameter("Find Prop", "When active, the prop will lit up 50% white fixed to be able to find it", false);
	findPropMode->setControllableFeedbackOnly(true);
	findPropMode->isSavable = false;
	addChildControllableContainer(&connectionCC);

	addChildControllableContainer(&sensorsCC);



	
	bakeStartTime = bakingCC.addFloatParameter("Bake Start Time", "Set the start time of baking", 0, 0, INT32_MAX, false);
	bakeStartTime->defaultUI = FloatParameter::TIME;
	bakeStartTime->canBeDisabledByUser = true;
	bakeEndTime = bakingCC.addFloatParameter("Bake End Time", "Set the end time of baking", 1, 1, INT32_MAX, false);
	bakeEndTime->defaultUI = FloatParameter::TIME;
	bakeEndTime->canBeDisabledByUser = true; 
	bakeFrequency = bakingCC.addIntParameter("Bake Frequency", "The frequency at which to bake", 100, 1, 50000, false);
	bakeFrequency->canBeDisabledByUser = true;
	
	bakeAndUploadTrigger = bakingCC.addTrigger("Bake and Upload", "Bake the current assigned block and upload it to the prop");
	bakeAndExportTrigger = bakingCC.addTrigger("Bake and Export", "Bake the current assigned block and export it to a file");

	bakeFileName = bakingCC.addStringParameter("Bake file name", "Name of the bake file to send and to play", "", false);
	bakeFileName->canBeDisabledByUser = true;

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

	createControllablesForContainer(params.getProperty("parameters", var()), this);

	startTimer(PROP_PING_TIMERID, 2000); //ping every 2s, expect a pong between thecalls
}

Prop::~Prop()
{
	clearItem();
}

void Prop::clearItem()
{
	colors.fill(Colours::black);
	sendColorsToProp();
	setBlockFromProvider(nullptr);
	if (family != nullptr) family->unregisterProp(this);
	signalThreadShouldExit();
	waitForThreadToExit(1000);
	setBlockFromProvider(nullptr);
}

void Prop::registerFamily(StringRef familyName)
{
	if (familyName.isEmpty()) return;

	PropFamily* f = PropManager::getInstance()->getFamilyWithName(familyName);
	if (f != nullptr)
	{
		family = f;
		family->registerProp(this);
	}
}

void Prop::setBlockFromProvider(LightBlockColorProvider* model)
{
	if (currentBlock == nullptr && model == nullptr) return;
	if (model != nullptr && currentBlock != nullptr && currentBlock->provider == model) return;
	if (isBaking->boolValue()) return;

	if (currentBlock != nullptr)
	{
		signalThreadShouldExit();
		waitForThreadToExit(100);

		removeChildControllableContainer(currentBlock.get());
		if (!currentBlock->provider.wasObjectDeleted())
		{
			currentBlock->provider->setHighlighted(false);

			unregisterLinkedInspectable(currentBlock->provider.get());
			currentBlock->provider->removeColorProviderListener(this);
			currentBlock->provider->removeInspectableListener(this);
		}

		//currentBlock->removeLightBlockListener(this);
		currentBlock = nullptr;
	}

	if (model != nullptr) currentBlock.reset(new LightBlock(model));


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock.get());
		currentBlock->provider->addInspectableListener(this);
		currentBlock->provider->addColorProviderListener(this);

		registerLinkedInspectable(currentBlock->provider.get());

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
		colors.fill(Colours::white.withBrightness(.5f));

		if (Engine::mainEngine != nullptr && !Engine::mainEngine->isClearing)
		{
			propListeners.call(&PropListener::colorsUpdated, this);
			propNotifier.addMessage(new PropEvent(PropEvent::COLORS_UPDATED, this));
		}
	}
	else if (currentBlock != nullptr)
	{
		double time = (Time::getMillisecondCounter() % (int)1e9) / 1000.0;
		colors = currentBlock->getColors(this, time, var());

		if (Engine::mainEngine != nullptr && !Engine::mainEngine->isClearing)
		{
			propListeners.call(&PropListener::colorsUpdated, this);
			propNotifier.addMessage(new PropEvent(PropEvent::COLORS_UPDATED, this));
		}
	}

	if (!bakeMode->boolValue() && !isBaking->boolValue() && !isUploading->boolValue()) sendColorsToProp();
	else if (seekBakeTime != -1)
	{
		seekBakePlaying(seekBakeTime);
		seekBakeTime = -1;
	}
}

void Prop::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider*>(activeProvider->targetContainer.get()));
	}
	else if (p == enabled)
	{
		if (!enabled->boolValue())
		{
			colors.fill(Colours::black);
			sendColorsToProp(true);
		}
	}
}

void Prop::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	if (c == resolution)
	{
		colors.resize(resolution->intValue());
	}
	else if (c == globalID)
	{
		propListeners.call(&PropListener::propIDChanged, this, previousID);
		previousID = globalID->intValue();
	}else if (c == bakeAndUploadTrigger || c == bakeAndExportTrigger)
	{
		initBaking(currentBlock.get(), c == bakeAndUploadTrigger ? UPLOAD : EXPORT);
	}
	else if (c == findPropMode)
	{
		colors.fill(findPropMode->boolValue() ? Colours::white : Colours::black);
		update();
	}
	else if (c == powerOffTrigger)
	{
		powerOffProp();
	}
	else if (c == restartTrigger)
	{
		restartProp();
	}
	else if (controllableFeedbackMap.contains(c))
	{
		sendControllableFeedbackToProp(c);
	}
}

void Prop::inspectableDestroyed(Inspectable* i)
{
	if (currentBlock != nullptr && i == currentBlock->provider) setBlockFromProvider(nullptr);
}


void Prop::sendColorsToProp(bool forceSend)
{
	if (!enabled->boolValue() && !forceSend) return;
	//if (!isConnected->boolValue() || !twoWayConnected->boolValue()) return; //tmp unblock that
	sendColorsToPropInternal();
}

void Prop::fillTypeOptions(EnumParameter* p)
{
	p->addOption("Club", CLUB)->addOption("Ball", BALL)->addOption("Poi", POI)->addOption("Hoop", HOOP)->addOption("Ring", RING)->addOption("Buggeng", BUGGENG)->addOption("Box", BOX);
}

void Prop::initBaking(BaseColorProvider* block, AfterBakeAction afterBakeAction)
{
	if (block == nullptr)
	{
		NLOGWARNING(niceName, "Current block not assigned, cannot bake");
		return;
	}

	afterBake = afterBakeAction;
	providerToBake = block;

	if (afterBake == EXPORT)
	{
		FileChooser fc("Export a block");
		if (fc.browseForFileToSave(true)) exportFile = fc.getResult();
		else return;
	}

	bakingProgress->setValue(0);
	uploadProgress->setValue(0);
	isBaking->setValue(true);
}

BakeData Prop::bakeBlock()
{
	if (providerToBake == nullptr) return BakeData();


	BakeData result = providerToBake->getBakeDataForProp(this);
	NLOG(niceName, "Baking block " << result.name);


	//overrides
	if(bakeFileName->enabled && bakeFileName->stringValue().isNotEmpty()) result.name = bakeFileName->stringValue();// currentBlock->shortName + "_" + globalID->stringValue();
	if(bakeStartTime->enabled) result.startTime = bakeStartTime->floatValue();
	if(bakeEndTime->enabled) result.endTime = bakeEndTime->floatValue();
	if(bakeFrequency->enabled) result.fps = bakeFrequency->intValue();

	if (!result.metaData.hasProperty("id"))
	{
		DBG("No ID set, default to global ID ");
		result.metaData.getDynamicObject()->setProperty("id", globalID->intValue());
	}
	else
	{
		DBG("ID already set " << result.metaData.getProperty("id", "[notset]").toString());
	}

	var params = new DynamicObject();
	params.getDynamicObject()->setProperty("updateAutomation", false); //Avoid actually updating the block parameter automations if there are some (in timeline for instance)
	params.getDynamicObject()->setProperty("sequenceTime", false); //Avoid using sequence's time

	MemoryOutputStream os(result.data, false);

	double stepTime = 1.0 / result.fps;
	for (double curTime = result.startTime; curTime <= result.endTime; curTime += stepTime)
	{
		if (threadShouldExit()) return result;

		if (providerToBake == nullptr) return result;
		Array<Colour> cols = providerToBake->getColors(this, curTime, params);
		for (auto& c : cols) os.writeInt(c.getARGB());
		result.numFrames++;

		bakingProgress->setValue(jmap<float>(curTime, result.startTime, result.endTime, 0, 1));
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
		seekBakeTime = (float)data;
		//if(seekBakeTime == -1) seekBakePlaying((float)data);
		break;

	case LightBlockColorProvider::BakeControl::STOP:
		stopBakePlaying();
		break;

	case LightBlockColorProvider::BakeControl::SHOW_ID:
		sendShowPropID((bool)data);
		break;
	}
}


void Prop::handleOSCMessage(const OSCMessage &m)
{
	if (m.getAddressPattern().toString() == "/pong") handlePong();
	else
	{
		OSCHelpers::findControllableAndHandleMessage(this, m, 1);
	}
}

void Prop::handlePong()
{
	receivedPongSinceLastPingSent = true;
}

void Prop::timerCallback(int timerID)
{
	switch (timerID)
	{
	case PROP_PING_TIMERID:
		isConnected->setValue(receivedPongSinceLastPingSent);
		sendPing();
		break;
	}
}


void Prop::createControllablesForContainer(var data, ControllableContainer* cc)
{
	if (data.isVoid()) return;

	NamedValueSet valueProps = data.getDynamicObject()->getProperties();
	for (auto& p : valueProps)
	{
		if (!p.value.isObject()) continue;

		if (p.value.getProperty("type", "") == "Container")
		{
			ControllableContainer* childCC = cc->getControllableContainerByName(p.name.toString(), true);

			if (childCC == nullptr)
			{
				int index = p.value.getProperty("index", -1);
				if (p.value.getProperty("canBeDisabled", false))
				{
					childCC = new EnablingControllableContainer(p.name.toString());
					((EnablingControllableContainer*)childCC)->enabled->setValue(p.value.getProperty("enabled", true));
				}
				else
				{
					childCC = new EnablingControllableContainer(p.name.toString());
				}
				cc->addChildControllableContainer(childCC, true, index);
			}

			childCC->editorIsCollapsed = p.value.getProperty("collapsed", false);
			createControllablesForContainer(p.value, childCC);

		}
		else
		{
			Controllable* c = getControllableForJSONDefinition(p.name.toString(), p.value);
			if (c == nullptr) continue;
			cc->addControllable(c);
		}
	}
}

Controllable* Prop::getControllableForJSONDefinition(const String& name, var def)
{
	String valueType = def.getProperty("type", "").toString();
	Controllable* c = ControllableFactory::createControllable(valueType);
	if (c == nullptr) return nullptr;

	c->setNiceName(name);

	DynamicObject* d = def.getDynamicObject();

	if (c->type != Controllable::TRIGGER)
	{
		if (def.hasProperty("min") || def.hasProperty("max")) ((Parameter*)c)->setRange(def.getProperty("min", INT32_MIN), def.getProperty("max", INT32_MAX));

		if (d->hasProperty("default")) ((Parameter*)c)->setValue(d->getProperty("default"));

		if (c->type == Controllable::ENUM)
		{
			EnumParameter* ep = dynamic_cast<EnumParameter*>(c);

			if (d->hasProperty("options") && d->getProperty("options").isObject())
			{
				NamedValueSet options = d->getProperty("options").getDynamicObject()->getProperties();
				for (auto& o : options)
				{
					ep->addOption(o.name.toString(), o.value);
				}
			}
			else
			{
				LOG("Options property is not valid : " << d->getProperty("options").toString());
			}
		}
		else if (c->type == Controllable::FLOAT)
		{
			FloatParameter* ep = dynamic_cast<FloatParameter*>(c);
			if (d->hasProperty("ui"))
			{
				String ui = d->getProperty("ui");
				if (ui == "slider") ep->defaultUI = FloatParameter::SLIDER;
				else if (ui == "stepper") ep->defaultUI = FloatParameter::STEPPER;
				else if (ui == "label") ep->defaultUI = FloatParameter::LABEL;
				else if (ui == "time") ep->defaultUI = FloatParameter::TIME;
			}
		}

		if (d->hasProperty("readOnly"))
		{
			c->setControllableFeedbackOnly(d->getProperty("readOnly"));
		}

		if (d->hasProperty("shortName"))
		{
			c->setCustomShortName(d->getProperty("shortName").toString());
		}

		if (d->hasProperty("description"))
		{
			c->description = d->getProperty("description").toString();
		}
	}

	return c;
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
			BakeData data = bakeBlock();

			if (threadShouldExit()) return;

			switch (afterBake)
			{
			case UPLOAD:
				isUploading->setValue(true);
				uploadBakedData(data);
				if (threadShouldExit()) return;
				isUploading->setValue(false);

				bakeMode->setValue(bakeMode->value, false, true); //force resend bakeMode
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
