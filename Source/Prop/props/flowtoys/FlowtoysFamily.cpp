/*
  ==============================================================================

	FlowtoysFamily.cpp
	Created: 5 Feb 2019 11:21:43am
	Author:  jonglissimo

  ==============================================================================
*/

#include "FlowtoysFamily.h"
#include "FlowtoysProp.h"
#include "flowclub/FlowClubProp.h"
#include "Prop/PropManager.h"

FlowtoysFamily::FlowtoysFamily() :
	PropFamily("Flowtoys"),
	clubCC("Clubs")
{
	autoAddSerialDevices = addBoolParameter("Auto add serial", "If checked, this will automatically create or assign connected serial device to corresponding props", true);
		
	targetPropFPS = addIntParameter("Prop FPS", "", 50, 1, 1000);

	addChildControllableContainer(&clubCC);
	irLevel = clubCC.addFloatParameter("IR Level", "IR LED brightness level", 0, 0, 1);
	
	SerialManager::getInstance()->addSerialManagerListener(this);
}

FlowtoysFamily::~FlowtoysFamily()
{
	SerialManager::getInstance()->removeSerialManagerListener(this);

}

void FlowtoysFamily::onContainerParameterChanged(Parameter * p)
{
	PropFamily::onContainerParameterChanged(p);

	if (p == targetPropFPS)
	{
		for (auto &prop : props)
		{
			FlowtoysProp * fp = dynamic_cast<FlowtoysProp *>(prop);
			if (fp != nullptr)
			{
				OSCMessage m("/strip/fps");
				m.addInt32(targetPropFPS->intValue());
				fp->oscSender.sendToIPAddress(fp->remoteHost->stringValue(), 9000, m);
			}
		}
	}
}

void FlowtoysFamily::onControllableFeedbackUpdate(ControllableContainer * cc, Controllable * c)
{
	PropFamily::onControllableFeedbackUpdate(cc, c);

	if (c == irLevel)
	{
		for (auto &prop : props)
		{
			if (FlowClubProp * fp = dynamic_cast<FlowClubProp *>(prop))
			{
				fp->irLevel->setValue(irLevel->floatValue());
			}
		}
	}
}

void FlowtoysFamily::portAdded(SerialDeviceInfo* info)
{
	if (autoAddSerialDevices->boolValue())
	{
		if (info->vid == FlowClubProp::vidFilter && info->pid == FlowClubProp::pidFilter) checkDeviceHardwareID(info);
	}
}

void FlowtoysFamily::portRemoved(SerialDeviceInfo* info)
{
	pendingDevices.removeAllInstancesOf(SerialManager::getInstance()->getPort(info,false));
}

void FlowtoysFamily::checkSerialDevices()
{
	for (auto& info : SerialManager::getInstance()->portInfos)
	{
		if (info->vid == FlowClubProp::vidFilter && info->pid == FlowClubProp::pidFilter) checkDeviceHardwareID(info);
	}
	
}

void FlowtoysFamily::checkDeviceHardwareID(SerialDeviceInfo* info)
{
	SerialDevice* d = SerialManager::getInstance()->getPort(info, true, 115200);
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

Prop * FlowtoysFamily::addPropForHardwareID(SerialDevice *device, String hardwareId, String type)
{
	Prop* p = PropManager::getInstance()->getPropWithHardwareId(hardwareId);
	if (p == nullptr)
	{
		p = static_cast<Prop*>(PropManager::getInstance()->managerFactory->create(type));
		if (p != nullptr)
		{
			p->deviceID = hardwareId;

			if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
			{
				bp->serialParam->setValueForDevice(device);
			}

			PropManager::getInstance()->addItem(p);
		}
	}
	else
	{
		LOG(p->deviceID << " already there, updating prop's remoteHost");
		if (BentoProp* bp = dynamic_cast<BentoProp*>(p)) bp->setSerialDevice(device);
	}
	
	return p;
}

void FlowtoysFamily::serialDataReceived(SerialDevice *d, const var& data)
{

	StringArray dataSplit;
	dataSplit.addTokens(data.toString(), true);
	if (dataSplit.size() == 0) return;
	if (dataSplit[0] == "wassup")
	{
		String fw = dataSplit[1];
		String type = dataSplit[2].removeCharacters("\"");
		LOG("Got wassup from " << d->info->description << " : " << fw);
		Prop * p = addPropForHardwareID(d, fw, type);
		if (p != nullptr)
		{
			d->removeSerialDeviceListener(this); //only remove after so it's not deleted
			pendingDevices.removeAllInstancesOf(d);
		}
	}
}

void FlowtoysFamily::timerCallback(int timerID)
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
