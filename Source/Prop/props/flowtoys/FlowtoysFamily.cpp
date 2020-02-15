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

}

FlowtoysFamily::~FlowtoysFamily()
{
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

void FlowtoysFamily::portRemoved(SerialDeviceInfo* d)
{
	//	
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
	d->writeString("yo\n");

	pendingDevices.add(d);

	startTimerHz(1);
}

void FlowtoysFamily::addPropForHardwareID(SerialDevice *device, String hardwareId)
{
	Prop* p = PropManager::getInstance()->getPropWithHardwareId(hardwareId);
	if (p == nullptr)
	{
		p = static_cast<Prop*>(PropManager::getInstance()->managerFactory->create(FlowClubProp::getTypeStringStatic()));
		if (p != nullptr)
		{
			p->deviceID = hardwareId;

			if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
			{
				bp->setSerialDevice(device);
			}

			LOG("Found " << p->type->getValueKey() << " with ID : " << p->deviceID);

			PropManager::getInstance()->addItem(p);
		}
	}
	else
	{
		LOG(p->deviceID << " already there, updating prop's remoteHost");
		if (BentoProp* bp = dynamic_cast<BentoProp*>(p)) bp->setSerialDevice(device);
	}
}

void FlowtoysFamily::serialDataReceived(SerialDevice *d, const var& data)
{
	StringArray dataSplit;
	dataSplit.addTokens("data", true);
	if (dataSplit.size() == 0) return;
	if (dataSplit[0] == "wassup")
	{
		String fw = dataSplit[1];
		LOG("Got wassup from " << d->info->description << " : " << fw);
		d->close();
		pendingDevices.removeAllInstancesOf(d);

		addPropForHardwareID(d, fw);
	}
}

void FlowtoysFamily::timerCallback()
{
	for (auto& d : pendingDevices) d->close();
	pendingDevices.clear();
}
