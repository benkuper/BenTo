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
#include "Prop/PropManager.h"

LighttoysFTProp::LighttoysFTProp(var params) :
	Prop(getTypeString(), "Lighttoys", params),
	device(nullptr),
	propsStatus("Connected Devices")
{
	deviceParam = new SerialDeviceParameter("device", "Serial device to connect", true);
	ioCC.addParameter(deviceParam);

	numPaired = generalCC.addIntParameter("Num Paired", "Number of paired props", 0, 0, 31);
	numConnected = generalCC.addIntParameter("Num Connected", "Number of connected props", 0, 0, 31);
	autoResolution = generalCC.addBoolParameter("Auto resolution", "If checked, the resolution will be automatically set depending on number of paired devices", true);
	
	
	isConnected = ioCC.addBoolParameter("Is Connected", "This is checked if a serial device is connected.", false);
	isConnected->setControllableFeedbackOnly(true);
	isConnected->isSavable = false;

	
	addNewPairing = ioCC.addTrigger("Add New Pairing Group", "Start pairing");
	addToGroup = ioCC.addTrigger("Add To Group", "Add to the existing paired group");
	finishPairing = ioCC.addTrigger("Finish Pairing", "Finish the pairing");

	bakingShowID = bakingCC.addIntParameter("Baking Show ID", "The ID of the show to upload to", 1, 1, 4);

	for (int i = 0; i < 32; i++)
	{
		FTPropStatus * ftp = new FTPropStatus(i);
		statusList.add(ftp);
		propsStatus.addChildControllableContainer(ftp);
	}

	//propsStatus.isSavable = false;
	addChildControllableContainer(&propsStatus);
		
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
		sendMessage("leach", getPropMaskForId(i), 6, colors[i].getBlue(), colors[i].getGreen(), colors[i].getRed(), colors[i].getBlue(), colors[i].getGreen(), colors[i].getRed());
	}
}

void LighttoysFTProp::uploadBakedData(BakeData data)
{
	if (data.numFrames == 0) return;

	//int showID = bakingShowID->intValue() - 1;// 1-4 > 0-3
	//int numFrames = bakedColors.size();
	//int numProps = resolution->intValue();
	
	/*
	for (int i = 0; i < numProps; i++)
	{
		//send
		//int propMask = getPropMaskForId(i);
		//sendMessage("ldson", propMask);
		//sendMessage("ldsclear", propMask, showID);
		//sleep(500); //todo, replace this by command "loprc", and check that it returns state of each process (example : 2:-1;4:0 means 2 is not complete, 4 is complete);

		//Array<int>

		String data = "SHOW_FOR_PROP_" + String(i) + "\n\n";

		for (auto &tc : bakedColors)
		{
			int r = tc.colors[i].getRed();
			int g = tc.colors[i].getGreen();
			int b = tc.colors[i].getBlue();
			int ms = int(tc.time * 1000);

			data += String(ms) + "ms: setrgb AB 0 > " + String(r) + "," + String(g) + "," + String(b) + "\n";

			//sendMessage("ldswrite",propMask, showID, 
			
		}

		data += String(int(bakedColors[bakedColors.size() - 1].time + 1)) + "s: end";

		DBG("Upload to prop " + data);

		File f = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("Show" + String(showID) + "_Prop" + String(i)+".lsf");
		if (f.existsAsFile()) f.deleteFile();

		{
			FileOutputStream fs(f);
			if (fs.openedOk())
			{
				fs.writeText(data,false,false,"\n");
				fs.flush();
				LOG("Show saved to file " << f.getFullPathName());
			}
			else
			{
				LOGWARNING("Error writing to file " << f.getFullPathName());
			}
		}

		

		//sendMessage("ldsoff", propMask);
	}
	*/

}

void LighttoysFTProp::onContainerParameterChangedInternal(Parameter * p)
{
	Prop::onContainerParameterChangedInternal(p);
	if (p == isConnected) battery->setValue(isConnected->boolValue() ? 1 : 0);
	if (p == deviceParam) setCurrentDevice(deviceParam->getDevice());
	else if (p == numPaired)
	{
		if (autoResolution->boolValue()) resolution->setValue(numPaired->intValue());
	}
}

void LighttoysFTProp::onContainerTriggerTriggered(Trigger * t)
{
	Prop::onContainerTriggerTriggered(t);
	if (t == addNewPairing) sendMessage("gadd 1");
	else if (t == addToGroup) sendMessage("gadd 0");
	else if (t == finishPairing) sendMessage("gstop");
}

void LighttoysFTProp::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	FTPropStatus * ftp = c->getParentAs<FTPropStatus>();
	if (ftp != nullptr)
	{
		if (c == ftp->flash)
		{
			sendMessage("lstop", getPropMaskForId(ftp->propID));
			sendMessage("gping",getPropMaskForId(ftp->propID));
		}
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
			float voltage = (double)(dataSplit[5].substring(4).getIntValue()) / 0x10000;
			//DBG("Voltage : " << voltage);
			
			slaveCheckList[propID] = isOn;
			
			statusList[propID]->isConnected->setValue(isOn);
			statusList[propID]->voltage->setValue(voltage);

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

void LighttoysFTProp::sendMessage(StringRef command, const int propMask, int numArgs, ...)
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
	//DBG("Send message : " << msg);
}

void LighttoysFTProp::timerCallback()
{
	//check connected devices
	memset(slaveCheckList, 0, 32);
	sendMessage("glist");
}

void LighttoysFTProp::autoDetectRemotes()
{
	for (auto &pi : SerialManager::getInstance()->portInfos)
	{
		if (pi->vid == 0x04D8 && pi->pid == 0x00DF)
		{
			DBG("Found device " << pi->uniqueDescription);
			bool alreadyExisting = false;
			for (auto &p : PropManager::getInstance()->items)
			{

				LighttoysFTProp * ftP = dynamic_cast<LighttoysFTProp *>(p);
				if (ftP != nullptr)
				{
					if (ftP->device != nullptr && ftP->device->info == pi)
					{
						alreadyExisting = true;
						break;
					}
				}
			}

			if (!alreadyExisting)
			{
				LOG("Found remote ! " << pi->uniqueDescription);
				LighttoysFTProp * ftP = new LighttoysFTProp(var());
				ftP->deviceParam->setValueWithKey(pi->uniqueDescription);
				PropManager::getInstance()->addItem(ftP);
			}
		}
	}
}
