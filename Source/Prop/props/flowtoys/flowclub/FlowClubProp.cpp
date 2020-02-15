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
	irCC("IR LED")
{
	resolution->setValue(32);

	serialParam->vidFilter = vidFilter;
	serialParam->pidFilter = pidFilter;

	button = sensorsCC.addBoolParameter("Button", "Is the button on the prop pressed ?", false);
	button->setControllableFeedbackOnly(true);

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
		OSCMessage m("/ir");
		m.addFloat32(irLevel->floatValue());
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
