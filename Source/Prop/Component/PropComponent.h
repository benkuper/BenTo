/*
  ==============================================================================

    PropComponent.h
    Created: 8 May 2020 3:06:15pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class Prop;

class PropComponent :
    public EnablingControllableContainer
{
public:
    PropComponent(Prop * prop, const String& name, bool canBeDisabled = false);
    virtual ~PropComponent();

    WeakReference<Prop> prop;
    //bool feedbackEnabled;
    //Array<Parameter*> sendValuesOnPropConnected;

    //Array<Controllable *> excludeControlControllables;

    //void sendControl(String control, var value = var());
    //virtual void onContainerParameterChanged(Parameter* p) override;
    //virtual void onContainerTriggerTriggered(Trigger* t) override;

    //virtual void update() {}

    //virtual void handlePropConnected();

    //virtual void handleMessage(const String &msg, var value = var());

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropComponent)

};