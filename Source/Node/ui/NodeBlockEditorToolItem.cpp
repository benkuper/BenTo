/*
  ==============================================================================

    NodelockEditorToolItem.cpp
    Created: 5 Mar 2019 10:24:09am
    Author:  bkupe

  ==============================================================================
*/


NodeBlockEditorToolItem::NodeBlockEditorToolItem(bool isParam, StringRef type) :
	isParam(isParam),type(type)
{
}

NodeBlockEditorToolItem::~NodeBlockEditorToolItem()
{
}

void NodeBlockEditorToolItem::paint(Graphics & g)
{
	Colour c = (isParam ? RED_COLOR : BLUE_COLOR).darker();
	g.setColour(c.brighter(isMouseOverOrDragging()?.5f:0));
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
	return Font(h - 2).getStringWidth(type) + 8;
}
