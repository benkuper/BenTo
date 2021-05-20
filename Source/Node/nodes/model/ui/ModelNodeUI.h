/*
  ==============================================================================

    ModelNodeUI.h
    Created: 5 Mar 2019 11:17:25am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class ModelNodeUI :
	public ColorNodeViewUI
{
public:
	ModelNodeUI(ModelNode * cn);
	~ModelNodeUI();

	ModelNode * modelNode;

	bool isDraggingItemOver;

	void paintOverChildren(Graphics &g) override;

	void itemDropped(const SourceDetails &source) override;
};