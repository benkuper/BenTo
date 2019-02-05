/*
  ==============================================================================

    PropFamily.h
    Created: 5 Feb 2019 11:25:04am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Prop.h"

class PropFamily :
	public EnablingControllableContainer
{
public:
	PropFamily(StringRef name, int updateRate = 60);
	~PropFamily();

	IntParameter * updateRate;

	Array<Prop *> props;

	void registerProp(Prop * p);
	void unregisterProp(Prop * p);

	void onContainerParameterChanged(Parameter * p) override;

};