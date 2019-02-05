/*
  ==============================================================================

	ColorNodeViewUI.cpp
	Created: 14 Apr 2018 5:07:30pm
	Author:  Ben

  ==============================================================================
*/

#include "ColorNodeViewUI.h"

ColorNodeViewUI::ColorNodeViewUI(ColorNode * cn) :
	NodeViewUI(cn),
	cn(cn)
{
}

ColorNodeViewUI::~ColorNodeViewUI()
{
}

void ColorNodeViewUI::paint(Graphics & g)
{
	NodeViewUI::paint(g);
	
	if (propToPreview == nullptr || cn == nullptr) return;

	int numLeds = propToPreview->resolution->intValue();
	float ratio = getWidth()*1.0f / getHeight();
	int ledSize = jmax((ratio > (1.0f / numLeds) ? getHeight() : getWidth()) / numLeds, 2);

	Rectangle<int> lr(getFeedbackBounds());
	lr = lr.withSizeKeepingCentre(ledSize, ledSize*numLeds);

	Array<Colour> colors = cn->getColors(propToPreview,Time::getMillisecondCounter()/1000.0f,var());
	for (int i = 0; i < numLeds; i++)
	{
		g.setColour(colors[i]);
		g.fillEllipse(lr.removeFromTop(ledSize).reduced(1).toFloat());
	}
}