/*
  ==============================================================================

	FlowClubProp.cpp
	Created: 10 Apr 2018 10:29:13pm
	Author:  Ben

  ==============================================================================
*/

#include "FlowClubProp.h"

FlowClubProp::FlowClubProp(var params) :
	FlowtoysProp(getTypeString(), params),
	irCC("IR LED"),
	buttonCC("Button"),
	batteryCC("Battery"),
	imuCC("IMU")
{
	resolution->setValue(32);

	serialParam->vidFilter = vidFilter;
	serialParam->pidFilter = pidFilter;

	batteryLevel = batteryCC.addFloatParameter("Level", "Level of battery", 1, 0, 1);
	batteryVoltage = batteryCC.addFloatParameter("Voltage", "Level of battery", 1, 0, 1);

	button = buttonCC.addBoolParameter("Pressed", "Is the button on the prop pressed ?", false);
	button->setControllableFeedbackOnly(true);

	shortPress = buttonCC.addTrigger("Short Press", "Short press detection");
	longPress = buttonCC.addTrigger("Long Press", "Short press detection");
	multiPress = buttonCC.addIntParameter("MultiPress", "MultiPress detection",0);

	imuUpdateRate = imuCC.addIntParameter("Update Rate", "Update rate for the IMU", 20, 1, 100);
	orientation = imuCC.addPoint3DParameter("Orientation", "Orientation of the prop, Yaw/Pitch/Roll in degrees");
	orientation->setBounds(-180, -90, -180, 180, 90, 180);

	sensorsCC.addChildControllableContainer(&batteryCC);
	sensorsCC.addChildControllableContainer(&buttonCC);
	sensorsCC.addChildControllableContainer(&imuCC);

	addChildControllableContainer(&irCC);
	irLevel = irCC.addFloatParameter("IR Level", "IR LED brightness level", 0, 0, 1);



}

FlowClubProp::~FlowClubProp()
{

}

void FlowClubProp::clearItem()
{
	FlowtoysProp::clearItem();
	setSerialDevice(nullptr);
}


void FlowClubProp::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	FlowtoysProp::onControllableFeedbackUpdateInternal(cc, c);

	if (c == irLevel)
	{
		OSCMessage m("/ir/level");
		m.addFloat32(irLevel->floatValue());
		oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
	}
	else if (c == imuUpdateRate)
	{
		OSCMessage m("/imu/updateRate");
		m.addInt32(imuUpdateRate->intValue());
		oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
	}
	else if (c == imuCC.enabled)
	{
		OSCMessage m("/imu/enabled");
		m.addInt32(imuCC.enabled->intValue());
		oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
	}
}

void FlowClubProp::serialDataReceived(SerialDevice* d, const var& data)
{
	StringArray dataSplit;
	dataSplit.addTokens(data.toString(), true);
	if (dataSplit.size() == 0) return;

	StringArray sourceSplit;
	sourceSplit.addTokens(dataSplit[0], ".", "\"");

	if (sourceSplit[0] == "bt")
	{
		LOG("Button Event " << sourceSplit[1] << " :  " << (dataSplit.size() > 1 ? dataSplit[1] : ""));
		if (sourceSplit[1] == "pressed") button->setValue(dataSplit[1].getIntValue() > 0);
	}
}
