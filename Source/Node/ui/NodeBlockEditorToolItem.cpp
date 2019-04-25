/*
  ==============================================================================

    NodelockEditorToolItem.cpp
    Created: 5 Mar 2019 10:24:09am
    Author:  bkupe

  ==============================================================================
*/

#include "NodeBlockEditorToolItem.h"

NodeBlockEditorToolItem::NodeBlockEditorToolItem(StringRef type) :
	type(type)
{

}

NodeBlockEditorToolItem::~NodeBlockEditorToolItem()
{
}

void NodeBlockEditorToolItem::paint(Graphics & g)
{
	g.setColour(BG_COLOR.brighter(isMouseOverOrDragging()?.5f:.2f));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
	g.setColour(TEXT_COLOR);
	g.setFont(getHeight() - 4);
	g.drawFittedText(type, getLocalBounds(), Justification::centred,1);
}

void NodeBlockEditorToolItem::mouseDrag(const MouseEvent &e)
{
	if (isDragAndDropActive()) return;
	Image dndImage = ImageCache::getFromMemory(BinaryData::new_png, BinaryData::new_pngSize).rescaled(40, 40);
	dndImage.multiplyAllAlphas(.5f);

	if (e.getDistanceFromDragStart() > 40)
	{
		var desc = new DynamicObject();
		desc.getDynamicObject()->setProperty("dataType", "NodeTool");
		desc.getDynamicObject()->setProperty("nodeType", type);
		startDragging(desc, this, dndImage, true);
	}
}

int NodeBlockEditorToolItem::getWidthForHeight(int h)
{
	return Font(h - 4).getStringWidth(type) + 4;
}
