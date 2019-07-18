/*
  ==============================================================================

    FlowClubProp.cpp
    Created: 10 Apr 2018 10:29:13pm
    Author:  Ben

  ==============================================================================
*/

#include "FlowClubProp.h"

FlowClubProp::FlowClubProp(var params) :
	FlowtoysProp(getTypeString(),params),
	irCC("IR LED")
{
	resolution->setValue(32);
	button = sensorsCC.addBoolParameter("Button", "Is the button on the prop pressed ?", false);
	button->setControllableFeedbackOnly(true);

	addChildControllableContainer(&irCC);
	irLevel = irCC.addFloatParameter("IR Level", "IR LED brightness level", 0, 0, 1);
}

FlowClubProp::~FlowClubProp()
{
}

void FlowClubProp::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	FlowtoysProp::onControllableFeedbackUpdateInternal(cc, c);

	if (c == irLevel)
	{
		OSCMessage m("/ir");
		m.addFloat32(irLevel->floatValue());
		oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
	}
}