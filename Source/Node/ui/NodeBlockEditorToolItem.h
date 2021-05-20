/*
  ==============================================================================

    NodelockEditorToolItem.h
    Created: 5 Mar 2019 10:24:09am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class NodeBlockEditorToolItem :
	public Component,
	public DragAndDropContainer
{
public:
	NodeBlockEditorToolItem(bool isParam, StringRef type);
	~NodeBlockEditorToolItem();

	bool isParam;
	String type;

	void paint(Graphics &g) override;
	void mouseDrag(const MouseEvent &e) override;
	
	int getWidthForHeight(int h);
};