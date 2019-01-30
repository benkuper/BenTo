/*
  ==============================================================================

    LighttoysFTProp.cpp
    Created: 30 Jan 2019 10:04:12am
    Author:  bkupe

  ==============================================================================
*/

#include "LighttoysFTProp.h"
#include <iostream>
#include <cstdarg>

LighttoysFTProp::LighttoysFTProp(var params) :
	Prop(getTypeString(), params),
	device(nullptr)
{
	sendRate = 30;

	deviceParam = new SerialDeviceParameter("device", "Serial device to connect", true);
	addParameter(deviceParam);

	isConnected = addBoolParameter("Is Connected", "This is checked if a serial device is connected.", false);
	isConnected->setControllableFeedbackOnly(true);
	isConnected->isSavable = false;

	numPaired = addIntParameter("Num Paired", "Number of paired props", 0, 0, 31);
	numConnected = addIntParameter("Num Connected", "Number of connected props", 0, 0, 31);
	autoResolution = addBoolParameter("Auto resolution", "If checked, the resolution will be automatically set depending on number of paired devices", true);

	SerialManager::getInstance()->addSerialManagerListener(this);

	startTimer(2000);
}

LighttoysFTProp::~LighttoysFTProp()
{
	if (SerialManager::getInstanceWithoutCreating() != nullptr)
	{
		SerialManager::getInstance()->removeSerialManagerListener(this);
	}

	setCurrentDevice(nullptr);
}

void LighttoysFTProp::sendColorsToPropInternal()
{
	const int numLeds = resolution->intValue();
	Array<uint8> data;

	for (int i = 0; i < numLeds; i++)
	{
		sendMessageToProps("leach", getPropMaskForId(i), 6, colors[i].getBlue(), colors[i].getGreen(), colors[i].getRed(), colors[i].getBlue(), colors[i].getGreen(), colors[i].getRed());
	}
}

void LighttoysFTProp::onContainerParameterChangedInternal(Parameter * p)
{
	Prop::onContainerParameterChangedInternal(p);
	if (p == deviceParam) setCurrentDevice(deviceParam->getDevice());
	else if (p == numPaired)
	{
		if (autoResolution->boolValue()) resolution->setValue(numPaired->intValue());
	}
}


void LighttoysFTProp::setCurrentDevice(SerialDevice * _device)
{
	if (device == _device) return;

	if (device != nullptr)
	{
		device->removeSerialDeviceListener(this);
	}

	device = _device;

	if (device != nullptr)
	{
		device->addSerialDeviceListener(this);
		device->open(115200);

		if (!device->isOpen())
		{
			NLOG(niceName, "Could not open device : " << device->info->port);
		}
		else
		{
			device->setMode(SerialDevice::PortMode::LINES);
		}

		lastOpenedDeviceID = device->info->deviceID;
	}

	isConnected->setValue(device != nullptr);

	//serialModuleListeners.call(&SerialModuleListener::currentdeviceChanged);
}



void LighttoysFTProp::portOpened(SerialDevice *)
{
	//serialModuleListeners.call(&SerialModuleListener::deviceOpened);
}

void LighttoysFTProp::portClosed(SerialDevice *)
{
	//serialModuleListeners.call(&SerialModuleListener::deviceClosed);
}

void LighttoysFTProp::portRemoved(SerialDevice *)
{
	setCurrentDevice(nullptr);
}

void LighttoysFTProp::serialDataReceived(const var & data)
{
	//DBG("Serial data received : " << data.toString());
	String msg = data.toString();
	if (msg.substring(0, 3) == "GRP")
	{
		StringArray dataSplit;
		dataSplit.addTokens(msg, ";", "\"");

		String pairingTarget = dataSplit[0].substring(6);

		if (pairingTarget == "master")
		{
			numPaired->setValue(dataSplit[2].substring(5).getIntValue());
			DBG("Num paired " << numPaired->intValue());
			
		}
		else if(pairingTarget == "slave")
		{
			int propID = dataSplit[1].substring(3).getIntValue();
			bool isOn = dataSplit[7].substring(8, 9) == "+";
			//float voltage = (double)(dataSplit[5].substring(4).getIntValue()) / 0x10000;
			//DBG("Voltage : " << voltage);
			//battery->setValue(voltage / 15);
			slaveCheckList[propID] = isOn;
			
			//propStatus[i]->connected->setValue(isOn);
			//propStatus[i]->voltage->setValue(voltage);

			if (propID == numPaired->intValue()-1)
			{
				int numConnectedDevices = 0;
				for (int i = 0; i < 32; i++)
				{
					if (!slaveCheckList[i]) { /* propStatus[i]->connected->setValue(false); */ }
					else numConnectedDevices++;
				}

				numConnected->setValue(numConnectedDevices);
			}
		}
	}
}

void LighttoysFTProp::portAdded(SerialDeviceInfo * info)
{
	//DBG("SerialModule, deviceAdded >" << info->hardwareID << "< > " << lastOpeneddeviceID);
	if (device == nullptr && lastOpenedDeviceID == info->deviceID)
	{
		setCurrentDevice(SerialManager::getInstance()->getPort(info));
	}
}

void LighttoysFTProp::portRemoved(SerialDeviceInfo * p)
{
	if (device != nullptr && device->info == p) setCurrentDevice(nullptr);
}

void LighttoysFTProp::sendMessageToProps(StringRef command, const int propMask, int numArgs, ...)
{
	if (device == nullptr) return;
	if (!device->isOpen()) return;

	String msg = command;
	if(propMask >= 0) msg += " " + String(propMask);

	if (numArgs > 0)
	{
		va_list vl;
		va_start(vl, numArgs);
		for (int i = 0; i < numArgs; i++)
		{
			msg += "," + String(va_arg(vl, int));
		}
		va_end(vl);

	}

	device->writeString(msg);
	DBG("Send message : " << msg);
}

void LighttoysFTProp::timerCallback()
{
	//check connected devices
	memset(slaveCheckList, 0, 32);
	sendMessageToProps("glist");
}
