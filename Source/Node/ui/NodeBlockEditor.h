/*
  ==============================================================================

    NodeBlockEditor.h
    Created: 13 Apr 2018 11:24:50pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlock/model/blocks/node/NodeBlock.h"
#include "NodeManagerUI.h"
#include "NodeBlockEditorToolbox.h"

class NodeBlockEditor :
	public ShapeShifterContentComponent,
	public Inspectable::InspectableListener,
	public EngineListener
{
public:
	NodeBlockEditor(const String &contentName);
	~NodeBlockEditor();

	NodeBlock * nodeBlock;
	WeakReference<Inspectable> nodeBlockRef;

	std::unique_ptr<NodeManagerUI> managerUI;
	
	NodeBlockEditorToolbox toolbox;

	void paint(Graphics &g) override;
	void resized() override;

	void setNodeBlock(NodeBlock * b);

	void inspectableDestroyed(Inspectable *) override;

	void endLoadFile() override;

	static const String getTypeStringStatic() { return "Node Editor"; }
	static NodeBlockEditor * create(const String &contentName = NodeBlockEditor::getTypeStringStatic()) { return new NodeBlockEditor(contentName); }
};