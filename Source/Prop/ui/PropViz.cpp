/*
  ==============================================================================

    PropViz.cpp
    Created: 11 Apr 2018 10:17:51pm
    Author:  Ben

  ==============================================================================
*/

#include "PropViz.h"
#include "Prop/Prop.h"

PropViz::PropViz(Prop * prop) :
	prop(prop),
	propRef(prop)
{
	prop->addAsyncCoalescedPropListener(this);
}

PropViz::~PropViz()
{
	if(!propRef.wasObjectDeleted()) prop->removeAsyncPropListener(this);
}

void PropViz::paint(Graphics & g)
{
	if (propRef.wasObjectDeleted() || prop->currentBlock == nullptr)
	{
		//g.setColour(TEXT_COLOR.darker());
		//g.drawFittedText("No active block", getLocalBounds().reduced(10), Justification::centred, 5);
		return;
	}

	int numLeds = prop->resolution->intValue();
	float ratio = getWidth()*1.0f / getHeight(); 
	int ledSize = jmax((ratio > (1.0f / numLeds) ? getHeight() : getWidth()) / numLeds, 1);
	
	Rectangle<int> lr(getLocalBounds());
	lr = lr.withSizeKeepingCentre(ledSize, ledSize*numLeds);

	for (int i = 0; i < numLeds; i++)
	{
		Rectangle<float> ledR = lr.removeFromTop(ledSize).reduced(1).toFloat();
		g.setColour(Colours::white.withAlpha(.15f));
		g.drawEllipse(ledR, .5f);
		g.setColour(prop->colors[i]);
		g.fillEllipse(ledR);
	}
}

void PropViz::newMessage(const Prop::PropEvent & e)
{
	switch (e.type)
	{
	case Prop::PropEvent::BLOCK_CHANGED:
		repaint();
		break;
	case Prop::PropEvent::COLORS_UPDATED:
		repaint();
		break;
	}
}
