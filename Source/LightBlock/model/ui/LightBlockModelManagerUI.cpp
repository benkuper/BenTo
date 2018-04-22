/*
  ==============================================================================

    LightBlockModelManagerUI.cpp
    Created: 10 Apr 2018 7:56:53pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelManagerUI.h"

LightBlockModelManagerUI::LightBlockModelManagerUI(const String &name, UserLightBlockModelManager * m) :
	BaseManagerShapeShifterUI(name, m)
{
	animateItemOnAdd = false;
	setSize(100, 100);

	addExistingItems();
}

LightBlockModelManagerUI::~LightBlockModelManagerUI()
{
}


void LightBlockModelManagerUI::paint(Graphics & g)
{
	Rectangle<int> hr = getLocalBounds().reduced(2).removeFromTop(headerHeight);
	g.setColour(BG_COLOR.brighter(.3f));
	g.fillRoundedRectangle(hr.toFloat(), 4);
	g.setColour(TEXT_COLOR);
	g.drawFittedText(manager->niceName, hr, Justification::centred, 1);
}

void LightBlockModelManagerUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	if (r.getWidth() == 0) return;
	addItemBT->setBounds(r.withSize(headerHeight, headerHeight).withX(r.getWidth() - headerHeight));

	r.translate(0, headerHeight + headerGap);

	if (itemsUI.size() > 0)
	{
		container.setBounds(getLocalBounds());
		viewport.setBounds(getLocalBounds());
	}

	int numThumbs = itemsUI.size();
	int numThumbPerLine = jmin(r.getWidth() / (thumbSize + gap), numThumbs);
	int numLines = ceil(numThumbs*1.f / numThumbPerLine);

	r.setHeight(numLines * (thumbSize + gap) - gap);

	int index = 0;
	int yIndex = 0;

	Rectangle<int> lr;

	for (auto &mui : itemsUI)
	{
		if (index % numThumbPerLine == 0)
		{

			int numThumbsInThisLine = jmin(numThumbs - index, numThumbPerLine);
			int lineWidth = numThumbsInThisLine * (thumbSize + gap) - gap;

			if (yIndex > 0) r.removeFromTop(gap);
			lr = r.removeFromTop(thumbSize);
			lr = lr.withSizeKeepingCentre(lineWidth, lr.getHeight());

			yIndex++;
		}

		mui->setBounds(lr.removeFromLeft(thumbSize));
		lr.removeFromLeft(gap);
		index++;
	}

	viewport.setBounds(getLocalBounds().withTrimmedTop(headerHeight+headerGap));
	container.setSize(getWidth(), r.getBottom());
}

