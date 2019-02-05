/*
  ==============================================================================

	PropFamily.cpp
	Created: 5 Feb 2019 11:25:04am
	Author:  jonglissimo

  ==============================================================================
*/

#include "PropFamily.h"

PropFamily::PropFamily(StringRef name, int _updateRate) :
	EnablingControllableContainer(name)
{
	updateRate = addIntParameter("Update Rate", "", _updateRate, 1, 500);
}

PropFamily::~PropFamily()
{
}

void PropFamily::registerProp(Prop * p)
{
	props.addIfNotAlreadyThere(p);
	p->updateRate = updateRate->intValue();
}

void PropFamily::unregisterProp(Prop * p)
{
	props.removeAllInstancesOf(p);
}

void PropFamily::onContainerParameterChanged(Parameter * p)
{
	if (p == updateRate)
	{
		for (auto &prop : props)
		{
			prop->updateRate = updateRate->intValue();
		}
	}
}
