/*
  ==============================================================================

    PropManager.cpp
    Created: 10 Apr 2018 6:59:29pm
    Author:  Ben

  ==============================================================================
*/

#include "PropManager.h"
juce_ImplementSingleton(PropManager)

#include "props/flowtoys/FlowtoysFamily.h"
#include "props/flowtoys/flowclub/FlowClubProp.h"

#include "props/garnav/GarnavFamily.h"
#include "props/garnav/SmarballProp.h"

#include "props/lighttoys/LighttoysFamily.h"
#include "props/lighttoys/ft/LighttoysFTProp.h"


PropManager::PropManager() :
	BaseManager("Props"),
	familiesCC("Families")
{
	managerFactory = &factory;

	detectProps = addTrigger("Detect Props", "Auto detect using the Yo protocol");
	autoAssignIdTrigger = addTrigger("Auto Assign IDs", "Auto assign based on order in the manager");

	
	String localIp = "";
	Array<IPAddress> ad;
	IPAddress::findAllAddresses(ad);
	for (auto &ip : ad)
	{
		if (ip.toString().startsWith("192.168.0.") || ip.toString().startsWith("192.168.1.") || ip.toString().startsWith("192.168.43.") || ip.toString().startsWith("10.1.10.") || ip.toString().startsWith("10.0.0."))
		{
			localIp = ip.toString(); 
			break;
		}
	}

	StringArray a;
	a.addTokens(localIp, ".", "\"");
	String broadcastIp = "192.168.1.255";
	if (a.size() == 4) broadcastIp = a[0] + "." + a[1] + "." + a[2] + ".255";

	localHost = addStringParameter("Local host", "Local IP to communicate with all clubs, should be automatically set but you can change it.", localIp);
	remoteHost = addStringParameter("Broadcast host", "Broadcast IP to communicate with all clubs", broadcastIp);

	addChildControllableContainer(&familiesCC);
	families.add(new FlowtoysFamily());
	families.add(new GarnavFamily());
	families.add(new LighttoysFamily());
	for (auto & f : families) familiesCC.addChildControllableContainer(f);

	factory.defs.add(Factory<Prop>::Definition::createDef("Flowtoys", "Flowtoys Creator Club", FlowClubProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Garnav", "Smartball", SmartballProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Lighttoys", "Lighttoys FT", LighttoysFTProp::create));

	receiver.addListener(this);

	setupReceiver();
	sender.connect("0.0.0.0", 9000);
}


PropManager::~PropManager()
{
}

void PropManager::setupReceiver()
{
	receiver.disconnect();
	bool result = receiver.connect(localPort);

	if (result) NLOG(niceName, "Now receiving on port : " << localPort);
	else NLOGERROR(niceName, "Error binding port " << localPort);


	Array<IPAddress> ad;
	IPAddress::findAllAddresses(ad);

	Array<String> ips;
	for (auto &a : ad) ips.add(a.toString());
	ips.sort();
	String s = "Local IPs:";
	for (auto &ip : ips) s += String("\n > ") + ip;

	NLOG(niceName, s);
}

Prop * PropManager::getPropWithHardwareId(const String &hardwareId)
{
	for (auto & p : items) if (p->deviceID == hardwareId) return p;
	return nullptr;
}

Prop * PropManager::getPropWithId(int id, Prop * excludeProp)
{
	for (auto & p : items)
	{
		if (p == excludeProp) continue;
		if (p->globalID->intValue() == id) return p;
	}
	return nullptr;
}

PropFamily * PropManager::getFamilyWithName(StringRef familyName)
{
	for (auto &p : families)
	{
		if (p->niceName == familyName) return p;
	}

	return nullptr;
}


void PropManager::onContainerTriggerTriggered(Trigger * t)
{
	if (t == autoAssignIdTrigger)
	{
		int id = 0;
		for (auto &p : items)
		{ 
			p->globalID->setValue(id);
			id++;
		}
	} else if (t == detectProps)
	{
		OSCMessage m("/yo");
		m.addArgument(localHost->stringValue());
		sender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
		LOG("Auto detecting props on " << remoteHost->stringValue() << "...");

		LighttoysFTProp::autoDetectRemotes();
	}
}



void PropManager::addItemInternal(Prop * p, var)
{
	p->addPropListener(this);
	if(items.size() > 1) p->globalID->setValue(getFirstAvailableID());
}

void PropManager::removeItemInternal(Prop * p)
{
	p->removePropListener(this);
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

void PropManager::propIDChanged(Prop * p, int previousID)
{
	Prop * otherPropWithSameID = getPropWithId(p->globalID->intValue(), p);
	if (otherPropWithSameID != nullptr) otherPropWithSameID->globalID->setValue(previousID);
}

void PropManager::oscMessageReceived(const OSCMessage & m)
{

	String address = m.getAddressPattern().toString();

	if (address == "/wassup")
	{
		String pHost = String(m[0].getString());
		String pid = String(m[1].getInt32());

		//ToDo : fix propID on ESP32 chips ot have proper hardwareID
		DBG("Got wassup : " << pHost);
		Prop * p = getPropWithHardwareId(pHost);
		if (p == nullptr)
		{
			FlowClubProp * fp = static_cast<FlowClubProp *>(managerFactory->create(FlowClubProp::getTypeStringStatic()));
			if (fp != nullptr)
			{
				fp->deviceID = pHost;
				fp->remoteHost->setValue(pHost);
				LOG("Found ! " << fp->deviceID << " : " << fp->remoteHost->stringValue());
				addItem(fp);
				autoAssignIdTrigger->trigger();
			} else
			{
				DBG("Type does not exist");
			}
		}
	} else if(address == "/battery/level")
	{
		String pid = String(m[0].getInt32());
		Prop * p = getPropWithHardwareId(pid);
		if (p == nullptr) return;
		p->battery->setValue(m[1].getFloat32());
	} else if (address == "/battery/charging")
	{
		String pid = String(m[0].getInt32());
		Prop * p = getPropWithHardwareId(pid);
		if (p == nullptr) return;
		p->battery->setValue(m[1].getFloat32());
	}
	else if (address == "/touch/pressed")
	{
		String pid = String(m[0].getInt32());
		FlowClubProp * fp = static_cast<FlowClubProp *>(getPropWithHardwareId(pid));
		if (fp == nullptr) return;
		fp->button->setValue(m[1].getInt32() == 1);
	} else
	{
		LOG("Message not handled : " << m.getAddressPattern().toString() << " >> " << m[0].getType() << " args");

	}
}