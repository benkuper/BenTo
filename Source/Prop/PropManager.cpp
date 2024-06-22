/*
  ==============================================================================

	PropManager.cpp
	Created: 10 Apr 2018 6:59:29pm
	Author:  Ben

  ==============================================================================
*/


#include "BentoEngine.h"
#include "Prop/PropIncludes.h"
#include "PropManager.h"

juce_ImplementSingleton(PropManager)

PropManager::PropManager() :
	BaseManager("Props"),
	//Thread("PropsDownload"),
	//familiesCC("Families"),
	connectionCC("Connection"),
	controlsCC("Controls"),
	playbackCC("Playback")
{
	saveAndLoadRecursiveData = true;

	managerFactory = &factory;
	selectItemWhenCreated = false;

	factory.defs.add(Factory<Prop>::Definition::createDef<Prop>(""));
	factory.defs.add(Factory<Prop>::Definition::createDef<BentoProp>(""));


	autoAddNetworkProps = connectionCC.addBoolParameter("Auto Add Network", "If checked, this will automatically add detected props on the network", false);
	autoAddUSBProps = connectionCC.addBoolParameter("Auto Add USB", "If checked, this will automatically add detected props connected through USB", false);
	detectProps = connectionCC.addTrigger("Detect Props", "Auto detect using the Yo protocol");
	addChildControllableContainer(&connectionCC);

	autoAssignIdTrigger = controlsCC.addTrigger("Auto Assign IDs", "Auto assign based on order in the manager");
	assignPropIdTrigger = controlsCC.addTrigger("Assign IDs from Props", "Assign Prop IDs from Prop to Global ID");
	//sendFeedback = controlsCC.addBoolParameter("Send Feedback", "If checked, will send feedback from sensor to OSC", false);
	clearAll = controlsCC.addTrigger("Clear all props", "Remove all props from manager");
	enableAll = controlsCC.addTrigger("Enable All", "");
	disableAll = controlsCC.addTrigger("Disable All", "");
	globalBrightness = controlsCC.addFloatParameter("Brightness", "Global Brightness of all props", .3f, 0, 1);
	disablePreview = controlsCC.addBoolParameter("Disable preview", "If checked, this will disable preview in prop UI, it reduces considerably the cpu/gpu consumption.", false);
	addChildControllableContainer(&controlsCC);

	uploadAll = playbackCC.addTrigger("Upload All", "Generate playback and upload to all props");
	playbackMode = playbackCC.addBoolParameter("Playback Mode", "Switch between live stream from the software, or playback from the prop's uploaded playback file.", false);
	powerOffAll = playbackCC.addTrigger("Poweroff All", "");
	resetAll = playbackCC.addTrigger("Reset All", "");


	fileName = playbackCC.addStringParameter("Playback Filename", "Filename of the playback", "sequence");
	loadAll = playbackCC.addTrigger("Load All", "Load playback on all devices that can play");
	playAll = playbackCC.addTrigger("Play All", "Play playback on all devices that can play");
	stopAll = playbackCC.addTrigger("Stop All", "Stop playback on all devices that can stop");
	deleteAllPlayback = playbackCC.addTrigger("Delete All Playback Files", "Delete all playback files on all devices");
	deleteAllScripts = playbackCC.addTrigger("Delete All Embedded Scripts", "Delete all Wasm scripts on all devices");

	loop = playbackCC.addBoolParameter("Loop Playback", "If checked, this will tell the player to loop the playing", false);
	addChildControllableContainer(&playbackCC);

	String localIp = NetworkHelpers::getLocalIP();

	//addChildControllableContainer(&familiesCC);



	receiver.addListener(this);

	setupReceiver();
	sender.connect("0.0.0.0", 9000);

	zeroconfSearcher = ZeroconfManager::getInstance()->addSearcher("OSCQuery", "_oscjson._tcp");
	zeroconfSearcher->addSearcherListener(this);

	SerialManager::getInstance()->addSerialManagerListener(this);

	//updatePropsAndFamiliesDefinitions();

	//if (factory.defs.size() == 0)
	//{
	//	startThread(); // if no props detected, check online
	//}
}


PropManager::~PropManager()
{
	SerialManager::getInstance()->removeSerialManagerListener(this);
	zeroconfSearcher->removeSearcherListener(this);
}

void PropManager::setupReceiver()
{
	receiver.disconnect();
	bool result = receiver.connect(localPort);

	if (result)
	{
		NLOG(niceName, "Now receiving on port : " << localPort);
		clearWarning();
	}
	else
	{
		NLOGERROR(niceName, "Error binding port " << localPort);
		setWarningMessage("Error binding port " + String(localPort));
	}

	Array<IPAddress> ad;
	IPAddress::findAllAddresses(ad);

	Array<String> ips;
	for (auto& a : ad) ips.add(a.toString());
	ips.sort();
	String s = "Local IPs:";
	for (auto& ip : ips) s += String("\n > ") + ip;


	NLOG(niceName, s);
}

Prop* PropManager::createPropIfNotExist(const String& type, const String& host, const String& id, const String& name)
{
	Prop* p = getPropWithDeviceID(id);
	if (p == nullptr)
	{
		p = static_cast<Prop*>(managerFactory->create(type));
		if (p == nullptr)
		{
			p = new BentoProp();
		}
		if (p != nullptr)
		{
			if (name.isNotEmpty()) p->setNiceName(name);
			p->deviceID = id;

			BentoProp* bp = dynamic_cast<BentoProp*>(p);
			if (bp != nullptr) bp->remoteHost->setValue(host);

			LOG("Found " << p->shape->getValueKey() << " with ID : " << p->deviceID);

			addItem(p);
			autoAssignIdTrigger->trigger();
		}
		else
		{
			DBG("Type does not exist " << type);
		}
	}
	else
	{
		LOG(p->deviceID << " already there, updating prop's remoteHost");
		BentoProp* bp = dynamic_cast<BentoProp*>(p);
		if (bp != nullptr) bp->remoteHost->setValue(host);
	}

	return p;
}

Prop* PropManager::getPropWithDeviceID(const String& deviceID)
{
	for (auto& p : items)
	{
		if (p->deviceID == deviceID) return p;
	}
	return nullptr;
}

Prop* PropManager::getPropWithId(int id, Prop* excludeProp)
{
	for (auto& p : items)
	{
		if (p == excludeProp) continue;
		if (p->globalID->intValue() == id) return p;
	}
	return nullptr;
}

//PropFamily* PropManager::getFamilyWithName(StringRef familyName)
//{
//	for (auto& p : families)
//	{
//		if (p->niceName == familyName) return p;
//	}
//
//	return nullptr;
//}


void PropManager::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
	BaseManager::onControllableFeedbackUpdate(cc, c);

	if (c == autoAssignIdTrigger)
	{
		int id = 0;
		for (auto& p : items)
		{
			p->globalID->setValue(id);
			id++;
		}
	}
	else if (c == assignPropIdTrigger) {
		LOG("Assign IDs from Prop IDs");

		for (auto& p : items)
		{
			if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
			{
				int propID = bp->componentsCC->getParameterByName("propID")->intValue();
				p->globalID->setValue(propID);
			}
		}
	}
	else if (c == detectProps)
	{
		Array<IPAddress> addresses;
		IPAddress::findAllAddresses(addresses);

		LOG("Auto detecting props");
		for (auto& ad : addresses)
		{
			String ip = ad.toString();
			StringArray a;
			a.addTokens(ip, ".", "\"");
			if (a.size() < 4) continue;
			String broadcastIP = a[0] + "." + a[1] + "." + a[2] + ".255";


			OSCMessage m("/yo");
			m.addArgument(ip);
			sender.sendToIPAddress(broadcastIP, 9000, m);
			LOG(" > sending /yo on " << broadcastIP << " with local ip " << ip << "...");
		}

		for (auto& s : zeroconfSearcher->services) addPropForService(s);

		checkSerialDevices();

	}
	else if (c == uploadAll)
	{
		for (auto& p : items) p->uploadPlaybackTrigger->trigger();
	}
	else if (c == powerOffAll)
	{
		for (auto& p : items) p->powerOffTrigger->trigger();
	}
	else if (c == resetAll)
	{
		for (auto& p : items) p->restartTrigger->trigger();
	}
	else if (c == clearAll)
	{
		Array<Prop*> itemsToRemove;
		itemsToRemove.addArray(items);
		removeItems(itemsToRemove);
	}
	else if (c == enableAll || c == disableAll)
	{
		for (auto& p : items) p->enabled->setValue(c == enableAll);

	}
	else if (c == globalBrightness)
	{
		for (auto& p : items) p->brightness->setValue(globalBrightness->floatValue());
	}
	else if (c == loadAll || c == playAll || c == stopAll)
	{
		for (auto& p : items)
		{
			if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
			{
				if (c == loadAll) bp->loadPlayback(fileName->stringValue());
				else if (c == playAll) bp->playPlayback(0, loop->boolValue());
				else if (c == stopAll) bp->stopPlaybackPlaying();
			}
		}
	}
	else if (c == playbackMode)
	{
		for (auto& pr : items) pr->playbackMode->setValue(playbackMode->boolValue());
	}
	else if (c == deleteAllPlayback || c == deleteAllScripts)
	{
		bool isPlayback = c == deleteAllPlayback;
		AlertWindow::showAsync(
			MessageBoxOptions().withIconType(AlertWindow::WarningIcon)
			.withTitle("Delete All files")
			.withMessage("Are you sure you want to delete all the files on all the props ?")
			.withButton("Yes")
			.withButton("No"),
			[&, isPlayback](int result)
			{
				if (result)
				{
					for (auto& p : items)
					{
						if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
						{
							bp->deleteFolder(isPlayback ? "/playback" : "/scripts");
						}
					}
				}
			});
	}
	else
	{
		Prop* p = ControllableUtil::findParentAs<Prop>(c);

		if (p != nullptr)
		{
			//bool shouldSend = sendFeedback->boolValue();

			/*
			if (shouldSend)
			{
				if (p->sensorsCC.containsControllable(c))
				{
					OSCMessage msg("/prop/" + String(p->globalID->intValue()) + c->getControlAddress(p));
					if (c->type != Controllable::TRIGGER) msg.addArgument(OSCHelpers::varToArgument(((Parameter*)c)->getValue()));

					BentoEngine* be = (BentoEngine*)Engine::mainEngine;
					be->globalSender.sendToIPAddress(be->remoteHost->stringValue(), be->remotePort->intValue(), msg);
				}
			}
			*/
		}
	}
}

void PropManager::addItemInternal(Prop* p, var)
{
	p->addPropListener(this);

	if (Engine::mainEngine->isLoadingFile) return;
	if (items.size() > 1) p->globalID->setValue(getFirstAvailableID());
}

void PropManager::addItemsInternal(Array<Prop*> props, var)
{
	for (auto& p : props)
	{
		if (p == nullptr) continue;
		p->addPropListener(this);

		if (Engine::mainEngine->isLoadingFile) continue;
		if (items.size() > 1) p->globalID->setValue(getFirstAvailableID());
	}
}


void PropManager::removeItemInternal(Prop* p)
{
	p->removePropListener(this);
}

void PropManager::removeItemsInternal(Array<Prop*> props)
{
	for (auto& p : props) p->removePropListener(this);
}

void PropManager::clear()
{
	BaseManager::clear();
	//for (auto& f : families) f->props.clear();
}

int PropManager::getFirstAvailableID()
{
	int numItems = items.size();
	for (int i = 0; i < numItems; i++)
	{
		if (getPropWithId(i) == nullptr) return i;
	}
	return numItems;
}

void PropManager::propIDChanged(Prop* p, int previousID)
{
	Prop* otherPropWithSameID = getPropWithId(p->globalID->intValue(), p);
	if (otherPropWithSameID != nullptr) otherPropWithSameID->globalID->setValue(previousID);
}

void PropManager::oscMessageReceived(const OSCMessage& m)
{
	String address = m.getAddressPattern().toString();

	if (address == "/wassup")
	{
		if (m.size() < 4)
		{
			LOGERROR("Not enough arguments for /wassup");
			return;
		}

		String pHost = OSCHelpers::getStringArg(m[0]);
		String pid = OSCHelpers::getStringArg(m[1]);
		String pType = m.size() > 2 ? OSCHelpers::getStringArg(m[2]) : BentoProp::getTypeStringStatic();
		String pName = m.size() > 3 ? OSCHelpers::getStringArg(m[3]) : "Prop";
		//String pType = m.size() >= 3 ? OSCHelpers::getStringArg(m[2]) : "Bento";

		LOG("Got wassup : " << pHost << " : " << pid << ", type is " << pType);
		createPropIfNotExist(pType, pHost, pid, pName);
	}
	//else  if (m.size() > 0 && m[0].isString())
	//{
	//	if (Prop* p = getPropWithDeviceID(OSCHelpers::getStringArg(m[0])))
	//	{
	//		p->handleOSCMessage(m);
	//	}
	//}
}

void PropManager::serviceAdded(ZeroconfManager::ServiceInfo* s)
{
	if (!autoAddNetworkProps->boolValue()) return;

}

void PropManager::addPropForService(ZeroconfManager::ServiceInfo* s)
{

	StringArray  nameSplit;
	nameSplit.addTokens(s->name, "-", "\"");

	String deviceType = s->getKey("deviceType");
	if (deviceType.isNotEmpty())
	{
		createPropIfNotExist(deviceType, s->ip, s->getKey("deviceID"), s->getKey("deviceName"));
	}

}

//void PropManager::updatePropsAndFamiliesDefinitions()
//{
//	factory.defs.clear();
//
//	File propFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/props");
//	Array<File> propFiles = propFolder.findChildFiles(File::TypesOfFileToFind::findFiles, true, "*.json");
//
//	for (auto& f : propFiles)
//	{
//		if (f.getParentDirectory().getFileName() == "families") continue; //not treating the families folder here
//
//		var pData = JSON::parse(f);
//		if (pData.isObject())
//		{
//			std::function<Prop* (var params)> createFunc = &Prop::create;
//			String propType = pData.getProperty("type", "");
//			if (propType == "Bento") createFunc = &BentoProp::create;
//
//			if (pData.hasProperty("vid") && pData.hasProperty("pid"))
//			{
//				vidpids.add({ pData.getProperty("vid","").toString().getHexValue32(),pData.getProperty("pid","").toString().getHexValue32() });
//			}
//
//
//			File fw = f.getParentDirectory().getChildFile("firmware.bin");
//			if (fw.existsAsFile()) pData.getDynamicObject()->setProperty("firmware", fw.getFullPathName());
//
//			factory.defs.add(FactorySimpleParametricDefinition<Prop>::createDef(pData.getProperty("menu", "").toString(), pData.getProperty("name", "").toString(), createFunc, pData));
//		}
//	}
//
//	for (auto& f : families) familiesCC.removeChildControllableContainer(f);
//	families.clear();
//
//	File familyFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/props/families");
//	if (!familyFolder.exists()) familyFolder.createDirectory();
//
//	Array<File> familyFiles = familyFolder.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.json");
//
//	for (auto& f : familyFiles)
//	{
//		var fData = JSON::parse(f);
//		if (fData.isObject())
//		{
//			PropFamily* fam = new PropFamily(fData);
//			families.add(fam);
//			familiesCC.addChildControllableContainer(fam, true);
//		}
//	}
//}

void PropManager::afterLoadJSONDataInternal()
{
	if (autoAddNetworkProps->boolValue())
	{
		for (auto& s : zeroconfSearcher->services) serviceAdded(s);
	}
}

//void PropManager::run()
//{
//	LOG("Updating prop definitions...");
//	URL url("https://benjamin.kuperberg.fr/bento/getProps.php");
//
//	std::unique_ptr<URL::DownloadTask> t = url.downloadToFile(File::getSpecialLocation(File::tempDirectory).getChildFile("props.zip"), URL::DownloadTaskOptions().withListener(this));
//	if (t != nullptr) propDownloadTask.reset(t.release());
//	else LOGERROR("Error downloading");
//}

//void PropManager::finished(URL::DownloadTask* task, bool success)
//{
//	File f = File::getSpecialLocation(File::tempDirectory).getChildFile("props.zip");
//	ZipFile zip(f);
//	zip.uncompressTo(File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName) + "/props"), true);
//
//	f.deleteFile();
//
//	updatePropsAndFamiliesDefinitions();
//
//	LOG("Prop definitions updated");
//}


// USB Detection
void PropManager::portAdded(SerialDeviceInfo* info)
{
	if (autoAddUSBProps->boolValue())
	{
		for (auto& vp : vidpids)
		{
			if (info->vid == vp.vid && info->pid == vp.pid) checkDeviceDeviceID(info);
		}
	}
}

void PropManager::portRemoved(SerialDeviceInfo* info)
{
	pendingDevices.removeAllInstancesOf(SerialManager::getInstance()->getPort(info, false));
}

void PropManager::checkSerialDevices()
{
	for (auto& info : SerialManager::getInstance()->portInfos)
	{
		for (auto& vp : vidpids)
		{
			if (info->vid == vp.vid && info->pid == vp.pid) checkDeviceDeviceID(info);
		}
	}

}

void PropManager::checkDeviceDeviceID(SerialDeviceInfo* info)
{
	SerialDevice* d = SerialManager::getInstance()->getPort(info, true);
	if (d == nullptr)
	{
		LOGWARNING("Port already opened : " << info->uniqueDescription);
		return;
	}

	d->setMode(SerialDevice::LINES);

	if (d->isOpen())
	{
		d->addSerialDeviceListener(this);

		startTimer(1, 200);
		startTimer(2, 1000);

		pendingDevices.addIfNotAlreadyThere(d);
	}
}

Prop* PropManager::addPropForHardwareID(SerialDevice* device, String deviceID, String type)
{
	Prop* p = PropManager::getInstance()->getPropWithDeviceID(deviceID);
	if (p == nullptr)
	{
		p = static_cast<Prop*>(PropManager::getInstance()->managerFactory->create(type));
		if (p != nullptr)
		{
			p->deviceID = deviceID;
			if (BentoProp* bp = dynamic_cast<BentoProp*>(p)) bp->serialParam->setValueFromDevice(device);
			PropManager::getInstance()->addItem(p);
		}
	}
	else
	{
		LOG(p->deviceID << " already there, updating prop's serial device");
		if (BentoProp* bp = dynamic_cast<BentoProp*>(p)) bp->serialParam->setValueFromDevice(device);
	}

	return p;
}

void PropManager::serialDataReceived(SerialDevice* d, const var& data)
{

	StringArray dataSplit;
	dataSplit.addTokens(data.toString(), true);
	if (dataSplit.size() == 0) return;
	if (dataSplit[0] == "wassup")
	{
		String fw = dataSplit[1];
		String type = dataSplit[2].removeCharacters("\"");
		LOG("Got wassup from " << d->info->description << " : " << fw);
		Prop* p = addPropForHardwareID(d, fw, type);
		if (p != nullptr)
		{
			d->removeSerialDeviceListener(this); //only remove after so it's not deleted 
			pendingDevices.removeAllInstancesOf(d);
		}
	}
}

void PropManager::timerCallback(int timerID)
{
	if (timerID == 1)
	{
		for (auto& d : pendingDevices)
		{
			d->writeString("yo\n");
		}
	}
	else if (timerID == 2)
	{
		for (auto& d : pendingDevices)
		{
			d->removeSerialDeviceListener(this);
		}
		pendingDevices.clear();
		stopTimer(1);
		stopTimer(2);
	}

}