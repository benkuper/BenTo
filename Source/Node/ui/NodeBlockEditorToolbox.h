/*
  ==============================================================================

	NodeBlockEditorToolbox.h
	Created: 5 Mar 2019 10:21:24am
	Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "NodeBlockEditorToolItem.h"

class NodeBlockEditorToolbox :
	public Component
{
public:
	NodeBlockEditorToolbox();
	~NodeBlockEditorToolbox();

	OwnedArray<NodeBlockEditorToolItem> items;

	void paint(Graphics &g) override;
	void resized() override;

	class  ToolboxListener
	{
	public:
		virtual ~ToolboxListener() {}
		virtual void askCreateNode(const String &nodeType) {}
	};


	ListenerList<ToolboxListener> toolboxLsteners;
	void addToolboxListener(ToolboxListener* newListener) { toolboxLsteners.add(newListener); }
	void removeToolboxListener(ToolboxListener* listener) { toolboxLsteners.remove(listener); }
};