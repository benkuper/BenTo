/*
  ==============================================================================

    PropComponent.h
    Created: 8 May 2020 3:06:15pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class Prop;

class PropComponent :
    public EnablingControllableContainer
{
public:
    PropComponent(Prop * prop, const String& name, bool canBeDisabled = false);
    virtual ~PropComponent();

    WeakReference<Prop> prop;

    void sendControl(String control, var value = var());
    virtual void onContainerParameterChanged(Parameter* p) override;

    virtual void handleMessage(const String &msg, var value = var());

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropComponent)

};