/*
  ==============================================================================

    PropManager.cpp
    Created: 10 Apr 2018 6:59:29pm
    Author:  Ben

  ==============================================================================
*/

#include "PropManager.h"
juce_ImplementSingleton(PropManager)

#include "props/flowtoys/flowball/FlowBallProp.h"
#include "props/flowtoys/flowclub/FlowClubProp.h"
#include "props/flowtoys/flowpoi/FlowPoiProp.h"
#include "props/garnav/SmarballProp.h"
#include "props/lighttoys/ft/LighttoysFTProp.h"
#include "props/lighttoys/visual/LighttoysVisualProp.h"

PropManager::PropManager() :
	BaseManager("Props")
{
	managerFactory = &factory;

	detectProps = addTrigger("Detect Props", "Auto detect using the Yo protocol");
	autoAssignIdTrigger = addTrigger("Auto Assign IDs", "Auto assign based on order in the manager");

	sendRate = addIntParameter("Send Rate", "", 1, 1, 200);
	targetPropFPS = addIntParameter("Prop FPS", "", 1, 1, 1000);

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


	factory.defs.add(Factory<Prop>::Definition::createDef("Flowtoys", "Flowtoys Creator Club", FlowClubProp::create));
	
	factory.defs.add(Factory<Prop>::Definition::createDef("Garnav", "Smartball", SmartballProp::create));

	factory.defs.add(Factory<Prop>::Definition::createDef("Lighttoys", "Lighttoys FT", LighttoysFTProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Lighttoys", "Lighttoys Visual", LighttoysVisualProp::create));
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
	for (auto & p : items) if (p->propId == hardwareId) return p;
	return nullptr;
}

Array<Prop *> PropManager::getPropsWithId(int id)
{
	if (id == -1) return Array<Prop *>(items.getRawDataPointer(), items.size());

	Array<Prop *> result;
	for (auto & p : items) if (p->id->intValue() == id) return p;
	return result;
}

void PropManager::onContainerParameterChanged(Parameter * p)
{
	if (p == sendRate)
	{
		for (auto &p : items)
		{
			FlowtoysProp * fp = dynamic_cast<FlowtoysProp *>(p);
			if (fp != nullptr) fp->sendRate->setValue(sendRate->intValue());
		}
	}
	else if (p == targetPropFPS)
	{
		for (auto &p : items)
		{
			FlowtoysProp * fp = dynamic_cast<FlowtoysProp *>(p); 
			if (fp != nullptr)
			{
				OSCMessage m("/strip/fps");
				m.addInt32(targetPropFPS->intValue());
				fp->oscSender.sendToIPAddress(fp->remoteHost->stringValue(), 9000 , m);
			}
		}
	}
}

void PropManager::onContainerTriggerTriggered(Trigger * t)
{
	if (t == autoAssignIdTrigger)
	{
		int id = 0;
		for (auto &p : items)
		{ 
			p->id->setValue(id);
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

void PropManager::oscMessageReceived(const OSCMessage & m)
{

	String address = m.getAddressPattern().toString();

	if (address == "/wassup")
	{
		String pHost = String(m[0].getString());
		String pid = String(m[1].getInt32());

		DBG("Got wassup : " << pHost);
		Prop * p = getPropWithHardwareId(pHost);
		if (p == nullptr)
		{
			FlowClubProp * fp = static_cast<FlowClubProp *>(managerFactory->create(FlowClubProp::getTypeStringStatic()));
			if (fp != nullptr)
			{
				fp->propId = pHost;
				fp->remoteHost->setValue(pHost);
				LOG("Found ! " << fp->propId << " : " << fp->remoteHost->stringValue());
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