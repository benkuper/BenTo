/*
  ==============================================================================

	FlowtoysFamily.cpp
	Created: 5 Feb 2019 11:21:43am
	Author:  jonglissimo

  ==============================================================================
*/

#include "FlowtoysFamily.h"
#include "FlowtoysProp.h"

FlowtoysFamily::FlowtoysFamily() :
	PropFamily("Flowtoys")
{
	targetPropFPS = addIntParameter("Prop FPS", "", 60, 1, 1000);
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
