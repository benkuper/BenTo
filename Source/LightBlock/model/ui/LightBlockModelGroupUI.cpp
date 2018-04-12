/*
  ==============================================================================

	LightBlockModelGroupUI.cpp
	Created: 10 Apr 2018 11:17:09pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelGroupUI.h"

LightBlockModelGroupUI::LightBlockModelGroupUI(ControllableContainer * group) :
	group(group)
{
	for (auto &cc : group->controllableContainers)
	{
		LightBlockModel * m = dynamic_cast<LightBlockModel *>(cc.get());
		if (m == nullptr) continue;
		LightBlockModelUI * mui = new LightBlockModelUI(m);
		modelsUI.add(mui);
		addAndMakeVisible(mui);
	}

}

LightBlockModelGroupUI::~LightBlockModelGroupUI()
{
}

void LightBlockModelGroupUI::paint(Graphics & g)
{
	Rectangle<int> hr = getLocalBounds().reduced(2).removeFromTop(headerHeight);
	g.setColour(BG_COLOR.brighter(.3f));
	g.fillRoundedRectangle(hr.toFloat(), 4);
	g.setColour(TEXT_COLOR);
	g.drawFittedText(group->niceName, hr, Justification::centred, 1);
}

void LightBlockModelGroupUI::resized()
{
	
	Rectangle<int> r = getLocalBounds().reduced(2).translated(0, headerHeight + headerGap);
	if (r.getWidth() == 0) return;

	int numThumbs = modelsUI.size();
	int numThumbPerLine = jmin(r.getWidth() / (thumbSize + gap), numThumbs);
	int numLines = ceil(numThumbs*1.f/ numThumbPerLine);
	
	r.setHeight(numLines * (thumbSize + gap) - gap);

	int index = 0;
	int yIndex = 0;

	Rectangle<int> lr;
	
	for (auto &mui : modelsUI)
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

	setSize(getWidth(),r.getBottom());
}
