/*
  ==============================================================================

    LightBlockClipUI.cpp
    Created: 17 Apr 2018 7:20:43pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipUI.h"

LightBlockClipUI::LightBlockClipUI(LightBlockClip * _clip) :
	BaseItemMinimalUI(_clip),
	Thread(_clip->niceName+"_preview"),
	imageIsReady(false)
{
	bgColor = BG_COLOR.brighter().withAlpha(.5f);
	generatePreview();
}

LightBlockClipUI::~LightBlockClipUI()
{
	signalThreadShouldExit();
	waitForThreadToExit(100);
}

void LightBlockClipUI::paint(Graphics & g)
{
	BaseItemMinimalUI::paint(g);
	if (!imageIsReady)
	{
		g.setColour(bgColor.brighter());
		g.drawText(item->currentBlock != nullptr?"Generating preview... ":"No Light block selected", getLocalBounds().reduced(8).toFloat(), Justification::centred);
	} else
	{
		g.setColour(Colours::white.withAlpha(.5f));
		g.drawImage(previewImage, getLocalBounds().toFloat());
	}
}

void LightBlockClipUI::resized()
{
	BaseItemMinimalUI::resized();
	if (!imageIsReady) generatePreview();
}

void LightBlockClipUI::generatePreview()
{
	imageIsReady = false;
	//repaint();

	signalThreadShouldExit();
	waitForThreadToExit(100);
	startThread();
}

void LightBlockClipUI::mouseDown(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseDown(e);
	timeAtMouseDown = item->startTime->floatValue();
	posAtMouseDown = getX();
}

void LightBlockClipUI::mouseDrag(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseDrag(e);
	clipUIListeners.call(&ClipUIListener::clipUIDragged, this, e);
}

void LightBlockClipUI::mouseUp(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseUp(e);
	item->startTime->setUndoableValue(timeAtMouseDown, item->startTime->floatValue());
}


void LightBlockClipUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	if (c == item->startTime || c == item->length)
	{
		clipUIListeners.call(&ClipUIListener::clipUITimeChanged, this);
		repaint();
	}

	if (c == item->length || c == item->activeProvider) generatePreview();
	if (item->currentBlock != nullptr && c->parentContainer == &item->currentBlock->paramsContainer) generatePreview();
}

void LightBlockClipUI::run()
{
	const int resX = jmin(getWidth(), 600);
	const int resY = 32; //to make dynamic

	if (resX == 0) return;
	previewImage = Image(Image::RGB, resX, resY, true);

	if (item->currentBlock == nullptr) return;

	imageIsReady = false;

	for (int i = 0; i < resX; i++)
	{
		if (threadShouldExit()) return;

		float relT = i * 1.0f / resX;
		float t = item->getTimeForRelativePosition(relT,false);
		Array<Colour> c = item->getColors(0, resY, t, var());
		for (int ty = 0; ty < resY; ty++) previewImage.setPixelAt(i, ty, c[ty]);
	}

	imageIsReady = true;
	
	MessageManagerLock mmLock;
	repaint();
}
