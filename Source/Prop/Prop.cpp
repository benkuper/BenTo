/*
  ==============================================================================

	Prop.cpp
	Created: 10 Apr 2018 6:59:25pm
	Author:  Ben

  ==============================================================================
*/

#include "BentoEngine.h"
#include "Prop.h"

Prop::Prop(var params) :
	BaseItem(params.getProperty("name", "Unknown").toString(), true, true),
	Thread("Prop"),
	family(nullptr),
	generalCC("General"),
	connectionCC("Connection"),
	controlsCC("Controls"),
	rgbComponent(nullptr),
	bakingCC("Bake and Upload"),
	pingEnabled(true),
	receivedPongSinceLastPingSent(false),
	providerToBake(nullptr),
	currentBlock(nullptr),
	previousID(-1),
	updateRate(50),
	propNotifier(50)
{
	registerFamily(params.getProperty("family", "Mistery Family").toString());

	customType = params.getProperty("type", "");

	logIncoming = addBoolParameter("Log Incoming", "Log all incoming messages from the prop", false);
	logOutgoing = addBoolParameter("Log Outgoing", "Log all outgoing messages to the prop", false);
	logIncoming->hideInEditor = true;
	logOutgoing->hideInEditor = true;

	showWarningInUI = true;
	editorIsCollapsed = true;
	saveAndLoadRecursiveData = true;

	addChildControllableContainer(&generalCC);
	globalID = generalCC.addIntParameter("Global ID", "The Global Prop ID, it is a unique ID but it can be swapped between props", 0, 0, 100);

	resolution = generalCC.addIntParameter("Resolution", "Number of controllable colors in the prop", params.getProperty("resolution", 1), 1);
	type = generalCC.addEnumParameter("Type", "The type of the prop");
	fillTypeOptions(type);
	type->setValueWithKey(params.getProperty("shape", type->getValueKey()));

	colors.resize(resolution->intValue());

	isConnected = connectionCC.addBoolParameter("Is Connected", "This is checked if the prop is connected.", false);
	isConnected->setControllableFeedbackOnly(true);
	isConnected->isSavable = false;

	findPropMode = connectionCC.addBoolParameter("Find Prop", "When active, the prop will lit up 50% white fixed to be able to find it", false);
	findPropMode->setControllableFeedbackOnly(true);
	findPropMode->isSavable = false;
	addChildControllableContainer(&connectionCC);


	powerOffTrigger = controlsCC.addTrigger("Power Off", "Power Off the prop");
	restartTrigger = controlsCC.addTrigger("Restart", "Restart the prop");
	addChildControllableContainer(&controlsCC);

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

	bakingCC.editorIsCollapsed = true;
	addChildControllableContainer(&bakingCC);


	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showSourcesAndGet;
	activeProvider->hideInEditor = true;

	setupComponentsJSONDefinition(params.getProperty("components", var()));

	var scriptsData = params.getProperty("scripts", var());
	for (int i = 0; i < scriptsData.size(); i++)
	{
		File propFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("BenTo/props");

		Script* script = scriptManager->addItem(nullptr, var(), false);
		scriptManager->setItemIndex(script, i);
		script->filePath->customBasePath = propFolder.getFullPathName();
		script->filePath->setControllableFeedbackOnly(true);
		script->filePath->setValue(scriptsData[i].toString());
		script->updateRate->setControllableFeedbackOnly(true);
		script->isSavable = false;
		script->userCanDuplicate = false;
		script->userCanRemove = false;
	}

	controllableContainers.move(controllableContainers.indexOf(scriptManager.get()), controllableContainers.size() - 1);

	pingEnabled = params.getProperty("ping", pingEnabled);
	if (pingEnabled) startTimer(PROP_PING_TIMERID, 2000); //ping every 2s, expect a pong between thecalls
}

Prop::~Prop()
{
	clearItem();
	masterReference.clear();
}

void Prop::clearItem()
{
	colors.fill(Colours::black);
	sendColorsToProp();
	setBlockFromProvider(nullptr);
	if (!Engine::mainEngine->isClearing && family != nullptr) family->unregisterProp(this);
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
	if (!enabled->boolValue()) return;

	HashMap<String, PropComponent*>::Iterator it(components);
	while (it.next()) it.getValue()->update();

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

		colorLock.enter();
		colors = currentBlock->getColors(this, time, var());
		colorLock.exit();

		if (Engine::mainEngine != nullptr && !Engine::mainEngine->isClearing)
		{
			propListeners.call(&PropListener::colorsUpdated, this);
			propNotifier.addMessage(new PropEvent(PropEvent::COLORS_UPDATED, this));
		}
	}


	if (!bakeMode->boolValue()
		&& !isBaking->boolValue()
		&& !isUploading->boolValue()
		&& (rgbComponent != nullptr && rgbComponent->streamEnable->boolValue()))
	{
		sendColorsToProp();
	}
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
	}
	else if (c == bakeAndUploadTrigger || c == bakeAndExportTrigger)
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
	else if (PropComponent* pc = dynamic_cast<PropComponent*>(cc)) //just do 1 level dynamic_cast<PropComponent*>(cc))
	{
		if (PropManager::getInstance()->sendFeedback->boolValue() && pc->feedbackEnabled)
		{
			OSCMessage m("/prop/" + globalID->stringValue() + c->getControlAddress(this));
			if (c->type != Controllable::TRIGGER) OSCHelpers::addArgumentsForParameter(m, (Parameter*)c);//PropManager::getInstance());

			BentoEngine* be = (BentoEngine*)Engine::mainEngine;
			PropManager::getInstance()->sender.sendToIPAddress(be->remoteHost->stringValue(), be->remotePort->intValue(), m);
		}
	}
	else if (c == isConnected)
	{
		if (isConnected->boolValue())
		{
			HashMap<String, PropComponent*>::Iterator it(components);
			while (it.next()) it.getValue()->handePropConnected();
		}
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
	p->addOption("Club", CLUB)->addOption("Ball", BALL)->addOption("Poi", POI)->addOption("Hoop", HOOP)->addOption("Ring", RING)->addOption("Buggeng", BUGGENG)->addOption("Box", BOX)->addOption("Custom", CUSTOM);
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
		FileChooser* fc(new FileChooser("Export a block"));
		fc->launchAsync(FileBrowserComponent::FileChooserFlags::saveMode, [this](const FileChooser& fc)
			{
				File f = fc.getResult();
				delete& fc;
				if (f == File()) return;

				exportFile = f;
				bakingProgress->setValue(0);
				uploadProgress->setValue(0);
				isBaking->setValue(true);
			}
		);
	}
	else
	{
		bakingProgress->setValue(0);
		uploadProgress->setValue(0);
		isBaking->setValue(true);
	}
}

BakeData Prop::bakeBlock()
{
	if (providerToBake == nullptr) return BakeData();


	BakeData result = providerToBake->getBakeDataForProp(this);
	NLOG(niceName, "Baking block " << result.name);


	//overrides
	if (bakeFileName->enabled && bakeFileName->stringValue().isNotEmpty()) result.name = bakeFileName->stringValue();// currentBlock->shortName + "_" + globalID->stringValue();
	if (bakeStartTime->enabled) result.startTime = bakeStartTime->floatValue();
	if (bakeEndTime->enabled) result.endTime = bakeEndTime->floatValue();
	if (bakeFrequency->enabled) result.fps = bakeFrequency->intValue();

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

	const int numLeds = resolution->intValue();
	bool invert = (rgbComponent != nullptr && rgbComponent->invertDirection);
	int startIndex = invert ? numLeds - 1 : 0;
	int endIndex = invert ? -1 : numLeds;
	int step = invert ? -1 : 1;

	double stepTime = 1.0 / result.fps;
	for (double curTime = result.startTime; curTime <= result.endTime; curTime += stepTime)
	{
		if (threadShouldExit()) return result;

		if (providerToBake == nullptr) return result;
		Array<Colour> cols = providerToBake->getColors(this, curTime, params);

		for (int i = startIndex; i != endIndex; i += step)
		{
			int index = (rgbComponent != nullptr && rgbComponent->useLayout) ? rgbComponent->ledIndexMap[i] : i;
			os.writeByte(cols[index].getRed());
			os.writeByte(cols[index].getGreen());
			os.writeByte(cols[index].getBlue());
		}
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

void Prop::addFileToUpload(File f)
{
	filesToUpload.add(f);
	isUploading->setValue(true);
}


void Prop::uploadFileQueue()
{
	while (filesToUpload.size() > 0)
	{
		File f = filesToUpload.removeAndReturn(0);
		uploadFile(f);
	}
}

void Prop::uploadFile(File f)
{
	NLOG(niceName, "Uploading file " + f.getFullPathName() + "...");
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

void Prop::sendControlToProp(String message, var value)
{
	if (!enabled->boolValue()) return;
	if (logOutgoing->boolValue())
	{
		NLOG(niceName, "Sending " + message + " : " + value.toString());
	}
	sendControlToPropInternal(message, value);
}


void Prop::handleOSCMessage(const OSCMessage& m)
{
	if (m.getAddressPattern().toString() == "/pong") handlePong();
	else if (m.getAddressPattern().toString() == "/enabled")
	{
		if (m.size() > 1) enabled->setValue(OSCHelpers::getIntArg(m[1]) == 1);
	}
	else if (m.getAddressPattern().toString() == "/model")
	{
		if (m.size() > 1)
		{
			if (LightBlockModel* p = dynamic_cast<LightBlockModel*>(LightBlockModelLibrary::getInstance()->getControllableContainerForAddress(OSCHelpers::getStringArg(m[1]))))
			{
				if (m.size() > 2)
				{
					if (LightBlockModelPreset* pr = p->presetManager.getItemWithName(OSCHelpers::getStringArg(m[2])))
					{
						activeProvider->setValueFromTarget(pr);
					}
				}
				else
				{
					activeProvider->setValueFromTarget(p);
				}
			}
		}
	}
	else
	{
		if (logIncoming->boolValue())
		{
			String s = "Received : " + m.getAddressPattern().toString() + ", ";
			for (int i = 1; i < m.size(); i++) s += "\n" + OSCHelpers::getStringArg(m[i]);
			NLOG(niceName, s);
		}

		StringArray mSplit;
		mSplit.addTokens(m.getAddressPattern().toString(), "/", "\"");

		if (mSplit[1] == "block")
		{
			if (currentBlock != nullptr && mSplit.size() > 2)
			{
				if (Controllable* c = currentBlock->paramsContainer.getControllableByName(mSplit[2]))
				{
					OSCHelpers::handleControllableForOSCMessage(c, m, 1);
				}
			}
		}
		else
		{
			PropComponent* pc = getComponent(mSplit[1]);
			var value;
			for (int i = 1; i < m.size(); i++) value.append(OSCHelpers::argumentToVar(m[i]));
			if (pc != nullptr) pc->handleMessage(mSplit[2], value);
		}

	}
}

void Prop::handlePong()
{
	if (!pingEnabled) return;
	receivedPongSinceLastPingSent = true;
	isConnected->setValue(true);
}

void Prop::sendPing()
{
	if (!pingEnabled) return;
	receivedPongSinceLastPingSent = false;
	sendPingInternal();
}

void Prop::timerCallback(int timerID)
{
	switch (timerID)
	{
	case PROP_PING_TIMERID:
		if (!pingEnabled) return;
		if (!receivedPongSinceLastPingSent) isConnected->setValue(false);
		sendPing();
		break;
	}
}

void Prop::setupComponentsJSONDefinition(var def)
{
	if (def.hasProperty("rgb"))
	{
		rgbComponent = new RGBPropComponent(this, def.getProperty("rgb", var()));
		addComponent(rgbComponent);
		resolution->setValue(rgbComponent->resolution);
		updateRate = rgbComponent->updateRate;
	}

	if (def.hasProperty("buttons")) addComponent(new ButtonsPropComponent(this, def.getProperty("buttons", var())));
	if (def.hasProperty("touch")) addComponent(new TouchPropComponent(this, def.getProperty("touch", var())));
	if (def.hasProperty("imu")) addComponent(new IMUPropComponent(this, def.getProperty("imu", var())));
	if (def.hasProperty("ir")) addComponent(new IRPropComponent(this, def.getProperty("ir", var())));
	if (def.hasProperty("battery")) addComponent(new BatteryPropComponent(this, def.getProperty("battery", var())));
	if (def.hasProperty("files")) addComponent(new FilesPropComponent(this, def.getProperty("files", var())));
}

void Prop::addComponent(PropComponent* pc)
{
	addChildControllableContainer(pc, true);
	components.set(pc->shortName, pc);
}

PropComponent* Prop::getComponent(const String& name)
{
	return components.contains(name) ? components[name] : nullptr;
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

InspectableEditor* Prop::getEditorInternal(bool isRoot, Array<Inspectable*> inspectables)
{
	return new PropEditor(this, isRoot);
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
		else if (filesToUpload.size() > 0)
		{
			isUploading->setValue(true);
			uploadFileQueue();
			isUploading->setValue(false);
		}
		else
		{
			update();
			sleep(1000.0f / updateRate); //50fps
		}
	}
}
