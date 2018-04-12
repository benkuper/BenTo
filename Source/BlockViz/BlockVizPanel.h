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
	public InspectableSelectionManager::Listener,
	public Inspectable::InspectableListener
{
public:

	BlockViz(const String &contentName = "Block Visualizer");
	~BlockViz();

	Prop vizProp;
	ScopedPointer<LightBlock> vizBlock;

	ScopedPointer<PropViz> propViz;

	ScopedPointer<ParameterUI> idUI;
	ScopedPointer<ParameterUI> resolutionUI;
	ScopedPointer<ParameterUI> shapeUI;

	LightBlock * currentBlock;
	
	WeakReference<Inspectable> blockRef;

	Rectangle<int> vizBounds;

	void resized() override;

	void setCurrentBlock(LightBlock * b);

	void inspectableDestroyed(Inspectable *) override;
	void inspectablesSelectionChanged() override;

	static BlockViz * create(const String &contentName) { return new BlockViz(contentName); }

};