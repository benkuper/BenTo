/*
  ==============================================================================

    LightBlockClipManagerUI.cpp
    Created: 17 Apr 2018 7:20:49pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipManagerUI.h"
#include "LightBlockLayerTimeline.h"

LightBlockClipManagerUI::LightBlockClipManagerUI(LightBlockLayerTimeline * _timeline, LightBlockClipManager * manager) :
	BaseManagerUI("Clip Manager", manager, false),
	timeline(_timeline)
{
	noItemText = "To add blocks to this layer, double-click here";
	addItemText = "Add Block";
	animateItemOnAdd = false;
	transparentBG = true;

	addItemBT->setVisible(false);
	addExistingItems();
}

LightBlockClipManagerUI::~LightBlockClipManagerUI()
{
}

void LightBlockClipManagerUI::resized()
{
	updateContent();
}

void LightBlockClipManagerUI::updateContent()
{
	for (auto &cui : itemsUI)
	{
		placeClipUI(cui);
	}
}

void LightBlockClipManagerUI::placeClipUI(LightBlockClipUI * cui)
{
	int tx = timeline->getXForTime(cui->item->time->floatValue());
	int tx2 = timeline->getXForTime(cui->item->time->floatValue() + cui->item->length->floatValue());
	cui->setBounds(tx, 0, tx2 - tx, getHeight());
}


void LightBlockClipManagerUI::addItemUIInternal(LightBlockClipUI * cui)
{
	cui->addClipUIListener(this);
	placeClipUI(cui);
}

void LightBlockClipManagerUI::removeItemUIInternal(LightBlockClipUI * cui)
{
	cui->removeClipUIListener(this);
}

void LightBlockClipManagerUI::mouseDoubleClick(const MouseEvent & e)
{
	manager->addClipAt(timeline->getTimeForX(e.getMouseDownX()));
}

void LightBlockClipManagerUI::clipUITimeChanged(LightBlockClipUI * cui)
{
	placeClipUI(cui);
}

void LightBlockClipManagerUI::clipUIDragged(LightBlockClipUI * cui, const MouseEvent & e)
{
	if (!e.mods.isShiftDown())
	{
		float targetTime = cui->timeAtMouseDown + timeline->getTimeForX(e.getOffsetFromDragStart().x, false);
		cui->item->time->setValue(targetTime);
	}
	repaint();
}
