/*
  ==============================================================================

    NodelockEditorToolItem.h
    Created: 5 Mar 2019 10:24:09am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class NodeBlockEditorToolItem :
	public Component,
	public DragAndDropContainer
{
public:
	NodeBlockEditorToolItem(StringRef type);
	~NodeBlockEditorToolItem();

	String type;

	void paint(Graphics &g) override;
	void mouseDrag(const MouseEvent &e) override;
	
	int getWidthForHeight(int h);
};