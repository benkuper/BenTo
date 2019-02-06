/*
  ==============================================================================

    PropClusterGroupManager.cpp
    Created: 5 Feb 2019 11:53:33am
    Author:  jonglissimo

  ==============================================================================
*/

#include "PropClusterGroupManager.h"
#include "Prop/PropManager.h"
#include "BentoEngine.h"

juce_ImplementSingleton(PropClusterGroupManager)

PropClusterGroupManager::PropClusterGroupManager() :
	BaseManager("Cluster Groups")
{
	PropManager::getInstance()->addControllableContainerListener(this);
}

PropClusterGroupManager::~PropClusterGroupManager()
{
	if(PropManager::getInstanceWithoutCreating() != nullptr) PropManager::getInstance()->removeControllableContainerListener(this);
}

void PropClusterGroupManager::controllableFeedbackUpdate(ControllableContainer * cc, Controllable * c)
{
	BaseManager::controllableFeedbackUpdate(cc, c);

	Prop * p = ControllableUtil::findParentAs<Prop>(c);
	if (p != nullptr)
	{
		bool shouldSend = c->parentContainer == &p->sensorsCC && c->type != Controllable::TRIGGER;
		
		if(shouldSend)
		{
			for (auto &i : items)
			{
				if (!i->sendFeedback->boolValue()) continue;

				int localID = -1;
				PropCluster * pc = i->getClusterForProp(p, localID);
				if (pc == nullptr) continue;
				
				OSCMessage msg("/cluster/" + i->shortName + "/" + pc->shortName + "/" + String(localID) + "/" + c->shortName);
				msg.addArgument(OSCHelpers::varToArgument(((Parameter *)c)->value));

				BentoEngine * be = (BentoEngine *)Engine::mainEngine;
				be->globalSender.sendToIPAddress(be->remoteHost->stringValue(), be->remotePort->intValue(), msg);
			}
		}
	}
}
