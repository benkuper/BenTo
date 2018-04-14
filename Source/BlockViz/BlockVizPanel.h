/*
  ==============================================================================

    BlockVizPanel.h
    Created: 11 Apr 2018 5:53:54pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Prop/Prop.h"
#include "Prop/ui/PropViz.h"


class BlockViz :
	public ShapeShifterContentComponent,
	public InspectableSelectionManager::AsyncListener,
	public Inspectable::InspectableListener
{
public:

	BlockViz(const String &contentName = "Block Visualizer");
	~BlockViz();

	Prop vizProp;
	LightBlock * vizBlock;

	ScopedPointer<PropViz> propViz;

	ScopedPointer<ParameterUI> idUI;
	ScopedPointer<ParameterUI> resolutionUI;
	ScopedPointer<ParameterUI> shapeUI;

	Rectangle<int> vizBounds;

	void resized() override;

	void newMessage(const InspectableSelectionManager::SelectionEvent &e) override;

	static BlockViz * create(const String &contentName) { return new BlockViz(contentName); }

};