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

FlowtoysFamily::FlowtoysFamily() :
	PropFamily("Flowtoys"),
	clubCC("Clubs")
{
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