/*
  ==============================================================================

	LightBlockClipUI.cpp
	Created: 17 Apr 2018 7:20:43pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipUI.h"
#include "../LightBlockLayer.h"
#include "LightBlock/model/ui/LightBlockModelUI.h"
#include "Prop/Prop.h"

LightBlockClipUI::LightBlockClipUI(LightBlockClip* _clip) :
	LayerBlockUI(_clip),
	Thread(_clip->niceName + "_preview"),
	clip(_clip),
	fadeInHandle(ImageCache::getFromMemory(BinaryData::fadeIn_png, BinaryData::fadeIn_pngSize)),
	fadeOutHandle(ImageCache::getFromMemory(BinaryData::fadeOut_png, BinaryData::fadeOut_pngSize)),
	shouldUpdateImage(true),
	shouldRepaint(false),
	imageIsReady(false),
	isDraggingModel(false)
{

	bgColor = Colours::transparentBlack;// BG_COLOR.brighter().withAlpha(.5f);

	previewProp.reset(new Prop());

	generatePreview();

	acceptedDropTypes.add("LightBlockModel");

	addChildComponent(&fadeInHandle, 0);
	addChildComponent(&fadeOutHandle, 0);

	fadeInHandle.setVisible(clip->fadeIn->enabled);
	fadeOutHandle.setVisible(clip->fadeOut->enabled);

	fadeInHandle.addMouseListener(this, false);
	fadeOutHandle.addMouseListener(this, false);

	clip->addAsyncClipListener(this);

	startTimerHz(10);
	startThread();
}

LightBlockClipUI::~LightBlockClipUI()
{
	if (!inspectable.wasObjectDeleted()) clip->removeAsyncClipListener(this);
	signalThreadShouldExit();
	waitForThreadToExit(500);
}

void LightBlockClipUI::paint(Graphics& g)
{
	bgColor = clip->currentBlock != nullptr ? Colours::transparentBlack : BG_COLOR.brighter().withAlpha(.5f);

	LayerBlockUI::paint(g);


	imgLock.enter();
	g.setColour(Colours::white.withAlpha(automationUI != nullptr ? .3f : .6f));
	g.drawImage(previewImage, getLocalBounds().toFloat(), RectanglePlacement::stretchToFit);
	imgLock.exit();

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
}

void LightBlockClipUI::paintOverChildren(Graphics& g)
{
	LayerBlockUI::paintOverChildren(g);

	Colour fInColor = (clip->fadeIn->enabled ? Colours::yellow : BLUE_COLOR).withAlpha(.5f);
	Colour fOutColor = (clip->fadeOut->enabled ? Colours::yellow : BLUE_COLOR).withAlpha(.5f);

	if (clip->fadeIn->floatValue() > 0)
	{
		g.setColour(fInColor);
		g.drawLine(0, getHeight(), getWidth() * (clip->fadeIn->floatValue() / clip->getTotalLength()), fadeInHandle.getY() + fadeInHandle.getHeight() / 2);
	}

	if (clip->fadeOut->floatValue() > 0)
	{
		g.setColour(fOutColor);
		g.drawLine(getWidth() * (1 - (clip->fadeOut->floatValue() / clip->getTotalLength())), fadeOutHandle.getY() + fadeOutHandle.getHeight() / 2, getWidth(), getHeight());
	}

	if (isDraggingModel) g.fillAll(BLUE_COLOR.withAlpha(.3f));
}

void LightBlockClipUI::resizedBlockInternal()
{
	if (!imageIsReady) generatePreview();
	if (automationUI != nullptr)
	{
		Rectangle<int> r = getCoreBounds();
		if (automationUI != nullptr)
		{
			if (dynamic_cast<GradientColorManagerUI*>(automationUI.get()) != nullptr) automationUI->setBounds(r.removeFromBottom(20));
			else automationUI->setBounds(r); 
		}
	}

	//Rectangle<int> r = getLocalBounds();
	fadeInHandle.setCentrePosition((clip->fadeIn->floatValue() / clip->getTotalLength()) * getWidth(), fadeInHandle.getHeight() / 2);
	fadeOutHandle.setCentrePosition((1 - clip->fadeOut->floatValue() / clip->getTotalLength()) * getWidth(), fadeOutHandle.getHeight() / 2);
}

void LightBlockClipUI::generatePreview()
{
	if (isMouseButtonDown()) return;
	shouldUpdateImage = true;
}

void LightBlockClipUI::setTargetAutomation(ParameterAutomation* a)
{
	if (automationUI != nullptr)
	{
		removeChildComponent(automationUI.get());
		automationUI = nullptr;
	}

	canBeGrabbed = true;

	if (a == nullptr) return;


	if (dynamic_cast<ParameterNumberAutomation*>(a) != nullptr)
	{
		AutomationUI* aui = new AutomationUI((Automation*)a->automationContainer);
		//aui->updateROI();
		aui->showMenuOnRightClick = false;
		automationUI.reset(aui);
	}
	else if (dynamic_cast<ParameterColorAutomation*>(a) != nullptr)
	{
		GradientColorManagerUI* gui = new GradientColorManagerUI((GradientColorManager*)a->automationContainer);
		gui->autoResetViewRangeOnLengthUpdate = true;
		automationUI.reset(gui);
	}

	if (automationUI != nullptr)
	{
		canBeGrabbed = false;
		coreGrabber.setVisible(false);
		grabber.setVisible(false);
		loopGrabber.setVisible(false);
		automationUI->addMouseListener(this, true);
		addAndMakeVisible(automationUI.get());
		resized();
		shouldRepaint = true;
	}
}

void LightBlockClipUI::mouseDown(const MouseEvent& e)
{
	LayerBlockUI::mouseDown(e);

	if (e.eventComponent == &fadeInHandle) fadeValueAtMouseDown = clip->fadeIn->floatValue();
	else if (e.eventComponent == &fadeOutHandle) fadeValueAtMouseDown = clip->fadeOut->floatValue();


	if (e.eventComponent == automationUI.get() && e.mods.isLeftButtonDown()) //because recursive mouseListener is removed to have special handling of automation
	{
		item->selectThis();
	}

	if (e.mods.isRightButtonDown() && (e.eventComponent == this || e.eventComponent == automationUI.get()))
	{
		if (clip->currentBlock != nullptr)
		{

			PopupMenu p;
			p.addItem(1, "Clear automation editor");

			PopupMenu ap;

			Array<WeakReference<Parameter>> params = clip->currentBlock->paramsContainer.getAllParameters();



			int index = 2;
			for (auto& pa : params)
			{
				if (pa->canBeAutomated) ap.addItem(index, pa->niceName, true, pa->controlMode == Parameter::ControlMode::AUTOMATION);
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
						pa->automation->setManualMode(true);
						Automation* a = dynamic_cast<Automation*>(pa->automation->automationContainer);
						if (a != nullptr)
						{
							a->clear();
							AutomationKey * k = a->addItem(0, 0);
							k->setEasing(Easing::BEZIER);
							a->addKey(a->length->floatValue(), 1);
							shouldUpdateImage = true;
						}
					}

					if (!pa.wasObjectDeleted()) setTargetAutomation(pa->automation.get());
				}
			}
		}
	}
}

void LightBlockClipUI::mouseDrag(const MouseEvent& e)
{
	LayerBlockUI::mouseDrag(e);

	if (e.eventComponent == &fadeInHandle)
	{
		clip->fadeIn->setValue((getMouseXYRelative().x * 1.0f / getWidth()) * clip->getTotalLength());
		resized();
	}
	else if (e.eventComponent == &fadeOutHandle)
	{
		clip->fadeOut->setValue((1 - (getMouseXYRelative().x * 1.0f / getWidth())) * clip->getTotalLength());
		resized();
	}
	else if (automationUI != nullptr)
	{
		generatePreview();
	}
}

void LightBlockClipUI::mouseUp(const MouseEvent& e)
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

void LightBlockClipUI::controllableFeedbackUpdateInternal(Controllable* c)
{
	LayerBlockUI::controllableFeedbackUpdateInternal(c);

	if (c == clip->coreLength || c == clip->loopLength || c == clip->activeProvider || c == clip->fadeIn || c == clip->fadeOut) generatePreview();
	else if (clip->currentBlock != nullptr && c->parentContainer == &clip->currentBlock->paramsContainer && c->type != Controllable::TRIGGER && ((Parameter*)c)->controlMode == Parameter::MANUAL) generatePreview();
	else if (c->getParentAs<AutomationKey>() != nullptr)
	{
		generatePreview();
	}
	else if (c->parentContainer->parentContainer == &clip->filters || c->parentContainer->parentContainer->parentContainer == &clip->filters) generatePreview();

	if (c == clip->fadeIn) fadeInHandle.setCentrePosition((clip->fadeIn->floatValue() / clip->getTotalLength()) * getWidth(), fadeInHandle.getHeight() / 2);
	else if (c == clip->fadeOut) fadeOutHandle.setCentrePosition((1 - clip->fadeOut->floatValue() / clip->getTotalLength()) * getWidth(), fadeOutHandle.getHeight() / 2);
}

void LightBlockClipUI::newMessage(const LightBlockClip::ClipEvent& e)
{
	switch (e.type)
	{
	case LightBlockClip::ClipEvent::FADES_CHANGED:
		fadeInHandle.setVisible(clip->fadeIn->enabled);
		fadeOutHandle.setVisible(clip->fadeOut->enabled);
		shouldRepaint = true;
		shouldUpdateImage = true;
		resized();
		break;

	case LightBlockClip::ClipEvent::REGENERATE_PREVIEW:
		shouldUpdateImage = true;
		break;
	}
}

void LightBlockClipUI::itemDropped(const SourceDetails& source)
{

	if (source.description.getProperty("dataType", "") == "LightBlockModel")
	{
		LightBlockModelUI* modelUI = dynamic_cast<LightBlockModelUI*>(source.sourceComponent.get());

		if (modelUI != nullptr)
		{
			LightBlockColorProvider* provider = modelUI->item;

			bool shift = KeyPress::isKeyCurrentlyDown(16);
			if (shift)
			{
				PopupMenu m;
				m.addItem(-1, "Default");
				m.addSeparator();
				int index = 1;
				for (auto& p : modelUI->item->presetManager.items) m.addItem(index++, p->niceName);
				int result = m.show();
				if (result >= 1) provider = modelUI->item->presetManager.items[result - 1];
			}

			if (LightBlockFilter* f = dynamic_cast<LightBlockFilter*>(provider)) clip->addFilterFromProvider(f);
			else clip->activeProvider->setValueFromTarget(provider, true);
		}
	}

	LayerBlockUI::itemDropped(source);
}

void LightBlockClipUI::timerCallback()
{
	if (shouldRepaint)
	{
		repaint();
		shouldRepaint = false;
	}
}

void LightBlockClipUI::run()
{
	while (!threadShouldExit())
	{
		sleep(20);

		if (!shouldUpdateImage || clip->currentBlock == nullptr)
		{
			sleep(20);
			continue;

		}

		shouldUpdateImage = false;
		imageIsReady = false;

		const int resX = jmin(getWidth(), 600);
		const int resY = 32; //to make dynamic

		if (resX == 0)
		{
			sleep(20);
			continue;
		}

		Image tmpImg = Image(Image::ARGB, resX, resY, true);

		int id = clip->layer->previewID->intValue();


		previewProp->globalID->setValue(id, true);
		previewProp->resolution->setValue(resY, true);

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

			Array<Colour> c = clip->getColors(previewProp.get(), absT, params);

			for (int ty = 0; ty < resY; ty++)
			{
				int py = resY - 1 - ty;
				if (relTotal > coreLength) c.set(py, c[ty].darker());// c[ty].darker();
				tmpImg.setPixelAt(i, py, c[ty]);
			}

		}

		imgLock.enter();
		previewImage = tmpImg.createCopy();
		imgLock.exit();
		imageIsReady = true;

		shouldRepaint = true;
	}
}

LightBlockFadeHandle::LightBlockFadeHandle(const Image& image) :
	img(image)
{
	setSize(14, 14);
}

void LightBlockFadeHandle::paint(Graphics& g)
{
	g.setColour(Colours::white);
	g.drawImage(img, getLocalBounds().toFloat().reduced(2));
}
