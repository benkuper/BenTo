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

	Prop::Shape shape = prop->type->getValueDataAsEnum<Prop::Shape>();
	switch (shape)
	{
	case Prop::Shape::CLUB:
	{
		float ratio = getWidth() * 1.0f / getHeight();
		int ledSize = jmax((ratio > (1.0f / numLeds) ? getHeight() : getWidth()) / numLeds, 1);

		Rectangle<int> lr(getLocalBounds());
		lr = lr.withSizeKeepingCentre(ledSize, ledSize * numLeds);

		for (int i = 0; i < numLeds; i++)
		{
			Rectangle<float> ledR = lr.removeFromTop(ledSize).reduced(1).toFloat();
			g.setColour(Colours::white.withAlpha(.2f));
			g.drawEllipse(ledR, .5f);
			g.setColour(prop->colors[i]);
			g.fillEllipse(ledR);
		}
	}
	break;

	case Prop::Shape::HOOP:
	{
		float size = jmin(getWidth(), getHeight()) - 8;
		Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(size, size);
		
		float radius = r.getWidth() / 2;
		float angle = float_Pi * 2 / numLeds;
		
		for (int i = 0; i < numLeds; i++)
		{
			Rectangle<float> lr(cosf(angle * i) * radius, sinf(angle * i) * radius,4,4);
			lr.translate(r.getCentreX()-2, r.getCentreY()-2);
			g.setColour(Colours::white.withAlpha(.2f));
			g.drawEllipse(lr , .5f);
			g.setColour(prop->colors[i]);
			g.fillEllipse(lr);
		}
	}
	break;
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
