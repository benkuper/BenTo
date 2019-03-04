/*
  ==============================================================================

    PropUI.h
    Created: 10 Apr 2018 7:00:05pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../Prop.h"
#include "PropViz.h"

class PropUI :
	public BaseItemUI<Prop>,
	public DragAndDropTarget
{
public:
	PropUI(Prop * p);
	~PropUI();

	PropViz viz;
	ScopedPointer<IntStepperUI> idUI;
	ScopedPointer<TargetParameterUI> targetUI;
	ScopedPointer<FloatSliderUI> batteryUI;

	bool acceptModelDrop;
	bool isDraggingItemOver;

	void paintOverChildren(Graphics &g) override;
	void mouseDown(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;

	void resizedInternalHeader(Rectangle<int> &r) override;
	void resizedInternalContent(Rectangle<int> &r) override;

	void controllableFeedbackUpdateInternal(Controllable * c) override;

	bool isInterestedInDragSource(const SourceDetails &source) override;
	void itemDragEnter(const SourceDetails &source) override;
	void itemDragExit(const SourceDetails &source) override;
	void itemDropped(const SourceDetails &source) override;
};