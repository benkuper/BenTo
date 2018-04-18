/*
  ==============================================================================

    LightBlockClipUI.cpp
    Created: 17 Apr 2018 7:20:43pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipUI.h"

LightBlockClipUI::LightBlockClipUI(LightBlockClip * _clip) :
	BaseItemUI(_clip)
{
	bgColor = BG_COLOR.darker();
}

LightBlockClipUI::~LightBlockClipUI()
{
}

void LightBlockClipUI::paint(Graphics & g)
{
	BaseItemUI::paint(g);

}

void LightBlockClipUI::resizedInternalHeader(Rectangle<int>& r)
{
}

void LightBlockClipUI::mouseDown(const MouseEvent & e)
{
	BaseItemUI::mouseDown(e);
	timeAtMouseDown = item->time->floatValue();
	posAtMouseDown = getX();
}

void LightBlockClipUI::mouseDrag(const MouseEvent & e)
{
	BaseItemUI::mouseDrag(e);
	clipUIListeners.call(&ClipUIListener::clipUIDragged, this, e);
}

void LightBlockClipUI::mouseUp(const MouseEvent & e)
{
	BaseItemUI::mouseUp(e);
	item->time->setUndoableValue(timeAtMouseDown, item->time->floatValue());
}

void LightBlockClipUI::buttonClicked(Button * b)
{
	BaseItemUI::buttonClicked(b);
}

void LightBlockClipUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	if (c == item->time || c == item->length)
	{
		clipUIListeners.call(&ClipUIListener::clipUITimeChanged, this);
		repaint();
	}
}