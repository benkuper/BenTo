/*
  ==============================================================================

	NodeBlockEditor.cpp
	Created: 13 Apr 2018 11:24:50pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeBlockEditor.h" 

NodeBlockEditor::NodeBlockEditor(const String & contentName) :
	ShapeShifterContentComponent(contentName),
	nodeBlock(nullptr),
	nodeBlockRef(nullptr)
{
	//InspectableSelectionManager::mainSelectionManager->addAsyncSelectionManagerListener(this);
	addAndMakeVisible(&toolbox);

	NodeBlock * b = InspectableSelectionManager::mainSelectionManager->getInspectableAs<NodeBlock>();
	if (b != nullptr) setNodeBlock(b);
}

NodeBlockEditor::~NodeBlockEditor()
{
	//if (InspectableSelectionManager::mainSelectionManager != nullptr) InspectableSelectionManager::mainSelectionManager->removeAsyncSelectionManagerListener(this);
	setNodeBlock(nullptr);
}

void NodeBlockEditor::paint(Graphics & g)
{
	if (managerUI == nullptr)
	{
		g.setColour(TEXT_COLOR);
		g.drawFittedText("Select a node block to edit it here", getLocalBounds().reduced(8), Justification::centred, 3);
	}
}

void NodeBlockEditor::resized()
{
	Rectangle<int> r = getLocalBounds();
	toolbox.setBounds(r.removeFromTop(20));

	if (managerUI != nullptr)
	{
		managerUI->setBounds(r);
	}
}

void NodeBlockEditor::setNodeBlock(NodeBlock * b)
{
	if (nodeBlock == b) return;

	if (nodeBlock != nullptr)
	{
		nodeBlock->removeInspectableListener(this);
		nodeBlock = nullptr;
		nodeBlockRef = nullptr;
		removeChildComponent(managerUI);
	}

	nodeBlock = b;

	if (nodeBlock != nullptr)
	{
		nodeBlock->addInspectableListener(this);
		nodeBlockRef = nodeBlock;
		managerUI = new NodeManagerUI(&nodeBlock->manager);
		addAndMakeVisible(managerUI);
	}

	repaint();
	resized();
}

void NodeBlockEditor::inspectableDestroyed(Inspectable * i)
{
	if (i == nodeBlock) setNodeBlock(nullptr);
}

void NodeBlockEditor::newMessage(const InspectableSelectionManager::SelectionEvent & e)
{
	/*
	if (e.type == InspectableSelectionManager::SelectionEvent::SELECTION_CHANGED)
	{
		NodeBlock * b = e.selectionManager->getInspectableAs<NodeBlock>();
		if (b != nullptr) setNodeBlock(b);
	}
	*/
}
