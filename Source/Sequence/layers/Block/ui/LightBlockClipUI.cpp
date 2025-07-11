/*
  ==============================================================================

	LightBlockClipUI.cpp
	Created: 17 Apr 2018 7:20:43pm
	Author:  Ben

  ==============================================================================
*/

#include "Sequence/SequenceIncludes.h"
#include "ParameterLink/ParameterLink.h"

LightBlockClipUI::LightBlockClipUI(LightBlockClip* _clip) :
	LayerBlockUI(_clip),
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
	acceptedDropTypes.add(SequenceBlock::getTypeStringStatic());
	acceptedDropTypes.add(StreamingScriptBlock::getTypeStringStatic());
	acceptedDropTypes.add(EmbeddedScriptBlock::getTypeStringStatic());
	acceptedDropTypes.add(PictureBlock::getTypeStringStatic());
	acceptedDropTypes.add(NodeBlock::getTypeStringStatic());
	acceptedDropTypes.add(VideoFileBlock::getTypeStringStatic());
	acceptedDropTypes.add(SharedTextureBlock::getTypeStringStatic());

	addChildComponent(&fadeInHandle, 0);
	addChildComponent(&fadeOutHandle, 0);

	fadeInHandle.setVisible(clip->fadeIn->enabled);
	fadeOutHandle.setVisible(clip->fadeOut->enabled);

	fadeInHandle.addMouseListener(this, false);
	fadeOutHandle.addMouseListener(this, false);

	clip->addAsyncClipListener(this);

	startTimerHz(10);
	LightBlockPreviewDispatcher::getInstance()->addClip(this);
}

LightBlockClipUI::~LightBlockClipUI()
{
	if(LightBlockPreviewDispatcher::getInstanceWithoutCreating()) LightBlockPreviewDispatcher::getInstance()->removeClip(this);
	if (!inspectable.wasObjectDeleted()) clip->removeAsyncClipListener(this);
}

void LightBlockClipUI::paint(Graphics& g)
{
	bgColor = clip->currentBlock != nullptr ? Colours::transparentBlack : BG_COLOR.brighter().withAlpha(.5f);

	LayerBlockUI::paint(g);


	imgLock.enter();
	g.setColour(Colours::white.withAlpha(automationUI != nullptr ? .3f : .6f));
	g.drawImage(previewImage, getCoreBounds().toFloat(), RectanglePlacement::stretchToFit);
	if (item->loopLength->floatValue() > 0)
	{
		g.setTiledImageFill(previewImage.rescaled(getCoreWidth(), getHeight(), Graphics::ResamplingQuality::lowResamplingQuality), getCoreWidth(), 0, .5f);
		g.fillRect(getLocalBounds().withLeft(getCoreWidth()));
	}
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

	if (clip->fadeIn->floatValue() > 0 && fadeInHandle.isVisible())
	{
		g.setColour(fInColor);
		int fadeInX = getRealXForTime(clip->fadeIn->floatValue());
		int zeroX = getRealXForTime(0);
		g.drawLine(zeroX, getHeight(), fadeInX, fadeInHandle.getY() + fadeInHandle.getHeight() / 2);
	}

	if (clip->fadeOut->floatValue() > 0 && fadeOutHandle.isVisible())
	{
		g.setColour(fOutColor);
		int fadeOutX = getRealXForTime(clip->getTotalLength() - clip->fadeOut->floatValue());
		int totalLengthX = getRealXForTime(clip->getTotalLength());
		g.drawLine(fadeOutX, fadeOutHandle.getY() + fadeOutHandle.getHeight() / 2, totalLengthX, getHeight());
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

	if (clip->fadeIn->enabled)
	{
		int fadeInX = getRealXForTime(clip->fadeIn->floatValue());
		fadeInHandle.setVisible(fadeInX >= 0);
		fadeInHandle.setCentrePosition(fadeInX, fadeInHandle.getHeight() / 2);
	}
	if (clip->fadeOut->enabled)
	{
		int fadeOutX = getRealXForTime(clip->getTotalLength() - clip->fadeOut->floatValue());
		fadeOutHandle.setVisible(fadeOutX <= getWidth());
		fadeOutHandle.setCentrePosition(fadeOutX, fadeOutHandle.getHeight() / 2);
	}
}

void LightBlockClipUI::generatePreview()
{
	if (isMouseButtonDown()) return;
	if (!isVisible()) return;
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
		if (AutomationUI* aui = dynamic_cast<AutomationUI*>(automationUI.get()))
		{
			automationUI->addMouseListener(this, false);
		}
		else if (GradientColorManagerUI* gui = dynamic_cast<GradientColorManagerUI*>(automationUI.get()))
		{

		}
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

	bool showAutomationMenu = e.eventComponent == this;
	if (AutomationUI* aui = dynamic_cast<AutomationUI*>(automationUI.get())) showAutomationMenu |= e.eventComponent == &aui->keysUI;
	//else if (GradientColorManagerUI* gui = dynamic_cast<GradientColorManagerUI*>(automationUI.get())) showAutomationMenu |= e.eventComponent == gui;

	if (e.mods.isRightButtonDown() && showAutomationMenu)
	{
		if (clip->currentBlock != nullptr)
		{

			PopupMenu p;

			if (automationUI != nullptr) p.addItem(1, "Clear automation editor");

			PopupMenu ap;

			Array<WeakReference<Parameter>> params = clip->currentBlock->paramsContainer->getAllParameters();



			int index = 2;
			for (auto& pa : params)
			{
				if (pa->canBeAutomated) ap.addItem(index, pa->niceName, true, pa->controlMode == Parameter::ControlMode::AUTOMATION);
				index++;
			}

			p.addSubMenu("Edit...", ap);

			p.showMenuAsync(PopupMenu::Options(), [this, params](int result)
				{
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
									AutomationKey* k = a->addItem(0, 0);
									k->setEasing(Easing::BEZIER);
									a->addKey(a->length->floatValue(), 1);
									shouldUpdateImage = true;
								}
							}

							if (!pa.wasObjectDeleted()) setTargetAutomation(pa->automation.get());
						}
					}
				}
			);
		}
	}
}

void LightBlockClipUI::mouseDrag(const MouseEvent& e)
{
	LayerBlockUI::mouseDrag(e);

	if (e.eventComponent == &fadeInHandle)
	{
		clip->fadeIn->setValue(getTimeForX(getMouseXYRelative().x));
		resized();
		repaint();
	}
	else if (e.eventComponent == &fadeOutHandle)
	{
		clip->fadeOut->setValue(clip->getTotalLength() - getTimeForX(getMouseXYRelative().x));
		resized();
		repaint();
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


void LightBlockClipUI::controllableFeedbackUpdateInternal(Controllable* c)
{
	LayerBlockUI::controllableFeedbackUpdateInternal(c);

	if (c == clip->coreLength || c == clip->loopLength || c == clip->activeProvider || c == clip->fadeIn || c == clip->fadeOut) generatePreview();
	else if (clip->currentBlock != nullptr && c->parentContainer == clip->currentBlock->paramsContainer.get() && c->type != Controllable::TRIGGER && ((Parameter*)c)->controlMode == Parameter::MANUAL) generatePreview();
	else if (c->getParentAs<AutomationKey>() != nullptr)
	{
		generatePreview();
	}
	else if (c->parentContainer->parentContainer == &clip->effects || c->parentContainer->parentContainer->parentContainer == &clip->effects) generatePreview();

	if (c == clip->fadeIn || c == clip->fadeOut) resized();
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
	if (LightBlockModelUI* modelUI = dynamic_cast<LightBlockModelUI*>(source.sourceComponent.get()))
	{
		if (modelUI != nullptr)
		{

			std::function<void(LightBlockColorProvider*, LightBlockModelUI*)> assignFunc = [this](LightBlockColorProvider* provider, LightBlockModelUI* modelUI)
				{
					if (LightBlockFilter* f = dynamic_cast<LightBlockFilter*>(provider)) clip->addEffectFromProvider(f);
					else
					{
						if (StreamingScriptBlock* sb = dynamic_cast<StreamingScriptBlock*>(provider))
						{
							PopupMenu m;
							m.addItem(1, "Replace source");
							m.addItem(2, "Add as effect");

							m.showMenuAsync(PopupMenu::Options(), [this, provider](int result)
								{
									if (result == 2)
									{
										clip->addEffectFromProvider(provider);
									}
									else
									{
										clip->activeProvider->setValueFromTarget(provider, true);
									}
								}
							);
						}
						else
						{
							clip->activeProvider->setValueFromTarget(provider, true);
						}
					}
				};

			bool shift = KeyPress::isKeyCurrentlyDown(16);
			if (shift)
			{
				PopupMenu m;
				m.addItem(-1, "Default");
				m.addSeparator();
				int index = 1;
				for (auto& p : modelUI->item->presetManager.items) m.addItem(index++, p->niceName);
				m.showMenuAsync(PopupMenu::Options(), [this, modelUI, assignFunc](int result)
					{
						if (result >= 1)
						{
							LightBlockColorProvider* provider = modelUI->item->presetManager.items[result - 1];
							assignFunc(provider, modelUI);
						}
					}
				);
			}
			else
			{
				assignFunc(modelUI->item, modelUI);
			}


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

void LightBlockClipUI::handleDrawPreview()
{
	if (!shouldUpdateImage || clip->currentBlock == nullptr) return;

	shouldUpdateImage = false;
	imageIsReady = false;

	const int resX = jmin(getCoreWidth(), 600);
	const int resY = 32; //to make dynamic

	if (resX == 0) return;

	Image tmpImg = Image(Image::ARGB, resX, resY, true);

	int id = clip->layer->previewID->intValue();


	previewProp->globalID->setValue(id, true);
	previewProp->resolution->setValue(resY, true);

	if (Prop* prop = PropManager::getInstance()->getPropWithId(id))
	{
		previewProp->customParams->loadJSONData(prop->customParams->getJSONData());
		//for (int i = 0; i < previewProp->customParams->controllables.size(); i++)
		//{
		//	((Parameter*)previewProp->customParams->controllables[i])->setValue(((Parameter*)prop->customParams->controllables[i])->getValue());

		//	DBG(previewProp->customParams->controllables[i]->niceName << " > " << ((Parameter*)previewProp->customParams->controllables[i])->stringValue());
		//}
	}

	var params = new DynamicObject();
	params.getDynamicObject()->setProperty("updateAutomation", false);
	//params.getDynamicObject()->setProperty("sequenceTime", false);

	float start = clip->time->floatValue();
	//float length = clip->getTotalLength();
	float coreLength = clip->coreLength->floatValue();

	for (int i = 0; i < resX; i++)
	{
		if (Thread::getCurrentThread()->threadShouldExit()) return;

		float relTotal = i * coreLength / resX;
		float absT = start + relTotal;

		Array<Colour> c = clip->getColors(previewProp.get(), absT, params);

		for (int ty = 0; ty < resY; ty++)
		{
			int py = resY - 1 - ty;
			if (relTotal > coreLength) c.set(ty, c[ty].darker());// c[ty].darker();
			tmpImg.setPixelAt(i, py, c[ty]);
		}

	}

	imgLock.enter();
	previewImage = tmpImg.createCopy();
	imgLock.exit();
	imageIsReady = true;

	shouldRepaint = true;
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


juce_ImplementSingleton(LightBlockPreviewDispatcher)

LightBlockPreviewDispatcher::LightBlockPreviewDispatcher() :
	Thread("LightBlockPreviewDispatcher")
{
	startThread();
}

LightBlockPreviewDispatcher::~LightBlockPreviewDispatcher()
{
	stopThread(1000);
}

void LightBlockPreviewDispatcher::run()
{
	while (!threadShouldExit())
	{
		wait(20); // 20ms is plenty enough
		if (Engine::mainEngine->isLoadingFile || Engine::mainEngine->isClearing) continue;

		for (auto& c : clips)
		{
			if (threadShouldExit()) return;

			if (c != nullptr && !c->inspectable.wasObjectDeleted())
			{
				c->handleDrawPreview();
			}
		}
	}
}

void LightBlockPreviewDispatcher::addClip(LightBlockClipUI* clipUI)
{
	clips.addIfNotAlreadyThere(clipUI);

}

void LightBlockPreviewDispatcher::removeClip(LightBlockClipUI* clipUI)
{
	clips.removeAllInstancesOf(clipUI);
}

