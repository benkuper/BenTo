/*
  ==============================================================================

    PropViz.h
    Created: 11 Apr 2018 10:17:51pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Prop/Prop.h"

class PropViz :
	public Component,
	public Prop::AsyncListener
{
public:
	PropViz(Prop * prop);
	~PropViz();

	Prop * prop;
	WeakReference<Inspectable> propRef;

	void paint(Graphics &g) override;

	void newMessage(const Prop::PropEvent &e) override;

};