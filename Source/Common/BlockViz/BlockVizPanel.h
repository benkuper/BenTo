/*
  ==============================================================================

    BlockVizPanel.h
    Created: 11 Apr 2018 5:53:54pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class Prop;
class PropViz;
class LightBlock;

class BlockViz :
	public ShapeShifterContentComponent,
	public InspectableSelectionManager::AsyncListener,
	public Inspectable::InspectableListener
{
public:

	BlockViz(const String &contentName = "Block Visualizer");
	~BlockViz();

	std::unique_ptr<Prop> vizProp;
	LightBlock * vizBlock;

	std::unique_ptr<PropViz> propViz;

	std::unique_ptr<ParameterUI> idUI;
	std::unique_ptr<ParameterUI> resolutionUI;
	std::unique_ptr<ParameterUI> shapeUI;

	Rectangle<int> vizBounds;

	void resized() override;

	void newMessage(const InspectableSelectionManager::SelectionEvent &e) override;

	static BlockViz * create(const String &contentName) { return new BlockViz(contentName); }

};