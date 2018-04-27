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
	removeMouseListener(this);
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
		g.drawText(item->currentBlock != nullptr?"Generating preview... ":"No Light block selected", getLocalBounds().reduced(8).toFloat(), Justification::centred);
	} else
	{
		g.setColour(Colours::white.withAlpha(automationUI != nullptr?.3f:.6f));
		g.drawImage(previewImage, getLocalBounds().toFloat());
	}

	if (automationUI != nullptr)
	{
		g.setFont(g.getCurrentFont().withHeight(jmin(getHeight() - 20, 20)).boldened());
		g.setColour(Colours::white.withAlpha(.5f));
		g.drawText("Editing " + automationUI->manager->parentContainer->niceName, getLocalBounds().reduced(10).toFloat(), Justification::centred);
	}
}

void LightBlockClipUI::resized()
{
	BaseItemMinimalUI::resized();
	if (!imageIsReady) generatePreview();
	if (automationUI != nullptr) automationUI->setBounds(getLocalBounds());
}

void LightBlockClipUI::generatePreview()
{
	imageIsReady = false;
	//repaint();

	signalThreadShouldExit();
	waitForThreadToExit(100);
	startThread();
}

void LightBlockClipUI::setTargetAutomation(Automation * a)
{
	if (automationUI != nullptr)
	{
		removeChildComponent(automationUI);
		automationUI = nullptr;
	}



	if (a == nullptr) return;

	automationUI = new AutomationUI(a);
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
		PopupMenu p;
		p.addItem(1, "Clear automation editor");

		PopupMenu ap;
		
		if (item->currentBlock != nullptr)
		{
			int index = 2;
			for (auto &i : item->currentBlock->automationsManager.items)
			{
				ap.addItem(index, i->niceName);	
				index++;
			}
		}

		p.addSubMenu("Edit...", ap);

		int result = p.show();
		if (result > 0)
		{
			if (result == 1) setTargetAutomation(nullptr);
			else setTargetAutomation(&item->currentBlock->automationsManager.items[result - 2]->automation);
		}
	}
}

void LightBlockClipUI::mouseDrag(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseDrag(e);
	if (e.eventComponent == this)
	{
		clipUIListeners.call(&ClipUIListener::clipUIDragged, this, e);
	}
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

	for (int i = 0; i < resX; i++)
	{
		if (threadShouldExit()) return;

		float relT = i * 1.0f / resX;
		float t = item->getTimeForRelativePosition(relT,false);
		Array<Colour> c = item->getColors(0, resY, t, params);
		for (int ty = 0; ty < resY; ty++) previewImage.setPixelAt(i, ty, c[ty]);
	}

	imageIsReady = true;
	
	MessageManagerLock mmLock;
	repaint();
}
