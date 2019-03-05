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
	public InspectableSelectionManager::AsyncListener,
	public Inspectable::InspectableListener
{
public:
	NodeBlockEditor(const String &contentName);
	~NodeBlockEditor();

	NodeBlock * nodeBlock;
	WeakReference<Inspectable> nodeBlockRef;

	ScopedPointer<NodeManagerUI> managerUI;
	
	NodeBlockEditorToolbox toolbox;

	void paint(Graphics &g) override;
	void resized() override;

	void setNodeBlock(NodeBlock * b);

	void inspectableDestroyed(Inspectable *) override;
	void newMessage(const InspectableSelectionManager::SelectionEvent &e) override;

	static NodeBlockEditor * create(const String &contentName) { return new NodeBlockEditor(contentName); }
};