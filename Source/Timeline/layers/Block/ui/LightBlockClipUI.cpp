/*
  ==============================================================================

	LightBlockClipUI.cpp
	Created: 17 Apr 2018 7:20:43pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipUI.h"
#include "../LightBlockLayer.h"

LightBlockClipUI::LightBlockClipUI(LightBlockClip * _clip) :
	BaseItemMinimalUI(_clip),
	Thread(_clip->niceName + "_preview"),
	fadeInHandle(ImageCache::getFromMemory(BinaryData::fadeIn_png, BinaryData::fadeIn_pngSize)),
	fadeOutHandle(ImageCache::getFromMemory(BinaryData::fadeOut_png, BinaryData::fadeOut_pngSize)),
    imageIsReady(false)
{
	bgColor = BG_COLOR.brighter().withAlpha(.5f);
	generatePreview();
	removeMouseListener(this);

	addAndMakeVisible(&fadeInHandle);
	addAndMakeVisible(&fadeOutHandle);
	fadeInHandle.addMouseListener(this, false);
	fadeOutHandle.addMouseListener(this, false);
}

LightBlockClipUI::~LightBlockClipUI()
{
	signalThreadShouldExit();
	waitForThreadToExit(300);
}

void LightBlockClipUI::paint(Graphics & g)
{
	BaseItemMinimalUI::paint(g);
	if (!imageIsReady)
	{
		g.setColour(bgColor.brighter());
		g.drawText(item->currentBlock != nullptr ? "Generating preview... " : "No Light block selected", getLocalBounds().reduced(8).toFloat(), Justification::centred);
	} else
	{
		g.setColour(Colours::white.withAlpha(automationUI != nullptr ? .3f : .6f));
		g.drawImage(previewImage, getLocalBounds().toFloat());
	}

	if (automationUI != nullptr)
	{
		g.setFont(g.getCurrentFont().withHeight(jmin(getHeight() - 20, 20)).boldened());
		g.setColour(Colours::white.withAlpha(.5f));
		//g.drawText("Editing " + automationUI->manager->parentContainer->niceName, getLocalBounds().reduced(10).toFloat(), Justification::centred);
	}

	if (item->fadeIn->floatValue() > 0)
	{
		g.setColour(Colours::yellow.withAlpha(.5f));
		g.drawLine(0, getHeight(), getWidth()*(item->fadeIn->floatValue() / item->length->floatValue()), 0);
	}

	if (item->fadeOut->floatValue() > 0)
	{
		g.setColour(Colours::yellow.withAlpha(.5f));
		g.drawLine(getWidth()*(1 - (item->fadeOut->floatValue() / item->length->floatValue())), 0, getWidth(), getHeight());
	}
}

void LightBlockClipUI::resized()
{
	BaseItemMinimalUI::resized();
	if (!imageIsReady) generatePreview();
	if (automationUI != nullptr) automationUI->setBounds(getLocalBounds());

	fadeInHandle.setCentrePosition((item->fadeIn->floatValue() / item->length->floatValue())*getWidth(), fadeInHandle.getHeight() / 2);
	fadeOutHandle.setCentrePosition((1 - item->fadeOut->floatValue() / item->length->floatValue())*getWidth(), fadeOutHandle.getHeight() / 2);
}

void LightBlockClipUI::generatePreview()
{
	if (isMouseButtonDown()) return;

	imageIsReady = false;
	//repaint();

	signalThreadShouldExit();
	waitForThreadToExit(100);
	startThread();
}

void LightBlockClipUI::setTargetAutomation(ParameterAutomation * a)
{
	if (automationUI != nullptr)
	{
		removeChildComponent(automationUI);
		automationUI = nullptr;
	}

	if (a == nullptr) return;

	automationUI = new AutomationUI(&a->automation);
	addAndMakeVisible(automationUI);
	resized();
	repaint();
	automationUI->updateROI();
	automationUI->addMouseListener(this, true);
}

void LightBlockClipUI::mouseDown(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseDown(e);
	timeAtMouseDown = item->startTime->floatValue();
	posAtMouseDown = getX();

	if (e.eventComponent == automationUI && e.mods.isLeftButtonDown()) //because recursive mouseListener is removed to have special handling of automation
	{
		item->selectThis();
	}

	if (e.mods.isRightButtonDown() && (e.eventComponent == this || e.eventComponent == automationUI))
	{
		if (item->currentBlock != nullptr)
		{

			PopupMenu p;
			p.addItem(1, "Clear automation editor");

			PopupMenu ap;

			Array<WeakReference<Parameter>> params = item->currentBlock->paramsContainer.getAllParameters();



			int index = 2;
			for (auto &pa : params)
			{
				ap.addItem(index, pa->niceName, true, pa->controlMode == Parameter::ControlMode::AUTOMATION);
				index++;
			}

			p.addSubMenu("Edit...", ap);

			int result = p.show();
			if (result > 0)
			{
				if (result == 1) setTargetAutomation(nullptr);
				else
				{
					WeakReference<Parameter> pa = params[result - 2];
					if (pa->controlMode != Parameter::ControlMode::AUTOMATION)
					{
						pa->setControlMode(Parameter::ControlMode::AUTOMATION);
						pa->automation->mode->setValueWithData(PlayableParameterAutomation::MANUAL);
					}

					if (!pa.wasObjectDeleted()) setTargetAutomation(pa->automation);
				}
			}
		}
	}
}

void LightBlockClipUI::mouseDrag(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseDrag(e);
	if (e.eventComponent == this)
	{
		clipUIListeners.call(&ClipUIListener::clipUIDragged, this, e);
	} else if (e.eventComponent == &fadeInHandle)
	{
		item->fadeIn->setValue((getMouseXYRelative().x * 1.0f / getWidth())*item->length->floatValue());
		resized();
	} else if (e.eventComponent == &fadeOutHandle)
	{
		item->fadeOut->setValue((1 - (getMouseXYRelative().x *1.0f / getWidth()))*item->length->floatValue());
		resized();
	}
}

void LightBlockClipUI::mouseUp(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseUp(e);
	item->startTime->setUndoableValue(timeAtMouseDown, item->startTime->floatValue());

	generatePreview();
}


void LightBlockClipUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	if (c == item->startTime || c == item->length)
	{
		clipUIListeners.call(&ClipUIListener::clipUITimeChanged, this);
		repaint();
	}

	if (c == item->length || c == item->activeProvider || c == item->fadeIn || c == item->fadeOut) generatePreview();
	else if (item->currentBlock != nullptr && c->parentContainer == &item->currentBlock->paramsContainer  && c->isControllableFeedbackOnly == false) generatePreview();
	else if (dynamic_cast<AutomationKey *>(c->parentContainer) != nullptr)
	{
		generatePreview();
	}
}

void LightBlockClipUI::run()
{
	const int resX = jmin(getWidth(), 600);
	const int resY = 32; //to make dynamic

	if (resX == 0) return;
	previewImage = Image(Image::RGB, resX, resY, true);

	if (item->currentBlock == nullptr) return;

	imageIsReady = false;

	var params = new DynamicObject();
	params.getDynamicObject()->setProperty("updateAutomation", false);
	
	int id = item->layer->targetId->intValue();
	for (int i = 0; i < resX; i++)
	{
		if (threadShouldExit()) return;

		float relT = i * 1.0f / resX;
		float t = item->getTimeForRelativePosition(relT, false);
		
		Array<Colour> c = item->getColors(id, resY, t, params);
		for (int ty = 0; ty < resY; ty++) previewImage.setPixelAt(i,ty, c[ty]);
	}

	imageIsReady = true;

	MessageManagerLock mmLock;
	repaint();
}

LightBlockFadeHandle::LightBlockFadeHandle(const Image & image) :
	img(image)
{
	setSize(12,12);
}

void LightBlockFadeHandle::paint(Graphics & g)
{
	g.setColour(Colours::white);
	g.drawImage(img, getLocalBounds().toFloat().reduced(2));
}
