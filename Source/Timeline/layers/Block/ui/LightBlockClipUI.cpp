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
	LayerBlockUI(_clip),
	Thread(_clip->niceName + "_preview"),
	clip(_clip),
	fadeInHandle(ImageCache::getFromMemory(BinaryData::fadeIn_png, BinaryData::fadeIn_pngSize)),
	fadeOutHandle(ImageCache::getFromMemory(BinaryData::fadeOut_png, BinaryData::fadeOut_pngSize)),
	imageIsReady(false)
{

	bgColor = BG_COLOR.brighter().withAlpha(.5f);
	generatePreview();
	
	//removeMouseListener(this);

	addAndMakeVisible(&fadeInHandle, 0);
	addAndMakeVisible(&fadeOutHandle, 0);
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
	LayerBlockUI::paint(g);

	if (previewImage.getWidth() > 0)
	{
		g.setColour(Colours::white.withAlpha(automationUI != nullptr ? .3f : .6f));
		g.drawImage(previewImage, getLocalBounds().toFloat(), RectanglePlacement::stretchToFit);
	}
	
	if (!imageIsReady)
	{
		g.setColour(bgColor.brighter());
		g.drawText(clip->currentBlock != nullptr ? "Generating preview... " : "No Light block selected", getCoreBounds().reduced(8).toFloat(), Justification::centred);
	}
	
	

	if (automationUI != nullptr)
	{
		g.setFont(g.getCurrentFont().withHeight(jmin(getHeight() - 20, 20)).boldened());
		g.setColour(Colours::white.withAlpha(.5f));
	}

	if (clip->fadeIn->floatValue() > 0)
	{
		g.setColour(Colours::yellow.withAlpha(.5f));
		g.drawLine(0, getHeight(), getWidth()*(clip->fadeIn->floatValue() / clip->getTotalLength()), fadeInHandle.getY()+fadeInHandle.getHeight()/2);
	}

	if (clip->fadeOut->floatValue() > 0)
	{
		g.setColour(Colours::yellow.withAlpha(.5f));
		g.drawLine(getWidth()*(1 - (clip->fadeOut->floatValue() / clip->getTotalLength())), fadeOutHandle.getY() + fadeOutHandle.getHeight() / 2, getWidth(), getHeight());
	}
}

void LightBlockClipUI::resizedBlockInternal()
{
	if (!imageIsReady) generatePreview();
	if (automationUI != nullptr) automationUI->setBounds(getCoreBounds());

	Rectangle<int> r = getLocalBounds();
	fadeInHandle.setCentrePosition((clip->fadeIn->floatValue() / clip->getTotalLength())*getWidth(), fadeInHandle.getHeight() / 2);
	fadeOutHandle.setCentrePosition((1 - clip->fadeOut->floatValue() / clip->getTotalLength())*getWidth(), fadeOutHandle.getHeight() / 2);
}

void LightBlockClipUI::generatePreview()
{
	if (isMouseButtonDown()) return;

	imageIsReady = false;

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
	LayerBlockUI::mouseDown(e);

	if (e.eventComponent == &fadeInHandle) fadeValueAtMouseDown = clip->fadeIn->floatValue();
	else if (e.eventComponent == &fadeOutHandle) fadeValueAtMouseDown = clip->fadeOut->floatValue();


	if (e.eventComponent == automationUI && e.mods.isLeftButtonDown()) //because recursive mouseListener is removed to have special handling of automation
	{
		item->selectThis();
	}

	if (e.mods.isRightButtonDown() && (e.eventComponent == this || e.eventComponent == automationUI))
	{
		if (clip->currentBlock != nullptr)
		{

			PopupMenu p;
			p.addItem(1, "Clear automation editor");

			PopupMenu ap;

			Array<WeakReference<Parameter>> params = clip->currentBlock->paramsContainer.getAllParameters();



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
	LayerBlockUI::mouseDrag(e);

	if (e.eventComponent == &fadeInHandle)
	{
		clip->fadeIn->setValue((getMouseXYRelative().x * 1.0f / getWidth())*clip->getTotalLength());
		resized();
	}
	else if (e.eventComponent == &fadeOutHandle)
	{
		clip->fadeOut->setValue((1 - (getMouseXYRelative().x *1.0f / getWidth()))*clip->getTotalLength());
		resized();
	}
	else if (automationUI != nullptr)
	{
		generatePreview();
	}
}

void LightBlockClipUI::mouseUp(const MouseEvent & e)
{
	LayerBlockUI::mouseUp(e);

	if (e.eventComponent == &fadeInHandle)
	{
		clip->fadeIn->setUndoableValue(fadeValueAtMouseDown, clip->fadeIn->floatValue());
		resized();
	}
	else if (e.eventComponent == &fadeOutHandle)
	{
		clip->fadeOut->setUndoableValue(fadeValueAtMouseDown, clip->fadeOut->floatValue());
		resized();
	}
}

Rectangle<int> LightBlockClipUI::getDragBounds()
{
	return LayerBlockUI::getLocalBounds();
}

void LightBlockClipUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	LayerBlockUI::controllableFeedbackUpdateInternal(c);

	if (c == clip->coreLength ||c == clip->loopLength || c == clip->activeProvider || c == clip->fadeIn || c == clip->fadeOut) generatePreview();
	else if (clip->currentBlock != nullptr && c->parentContainer == &clip->currentBlock->paramsContainer) generatePreview();
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
	Image tmpImg = Image(Image::RGB, resX, resY, true);

	if (clip->currentBlock == nullptr) return;

	imageIsReady = false;

	int id = clip->layer->previewID->intValue();

	previewProp.globalID->setValue(id, true);
	previewProp.resolution->setValue(resY, true);

	var params = new DynamicObject();
	params.getDynamicObject()->setProperty("updateAutomation", false);

	float start = clip->time->floatValue();
	float length = clip->getTotalLength();
	float coreLength = clip->coreLength->floatValue();

	for (int i = 0; i < resX; i++)
	{
		if (threadShouldExit()) return;


		float relTotal = i * length / resX;
		float absT = start + relTotal;
		Array<Colour> c = clip->getColors(&previewProp, absT, params);
		for (int ty = 0; ty < resY; ty++)
		{
			if (relTotal > coreLength) c.set(ty, c[ty].darker());// c[ty].darker();
			tmpImg.setPixelAt(i, ty, c[ty]);
		}
	}

	previewImage = tmpImg.createCopy();

	imageIsReady = true;

	MessageManagerLock mmLock;
	repaint();
}

LightBlockFadeHandle::LightBlockFadeHandle(const Image & image) :
	img(image)
{
	setSize(12, 12);
}

void LightBlockFadeHandle::paint(Graphics & g)
{
	g.setColour(Colours::white);
	g.drawImage(img, getLocalBounds().toFloat().reduced(2));
}
