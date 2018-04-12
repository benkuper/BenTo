/*
  ==============================================================================

	BlockVizPanel.cpp
	Created: 11 Apr 2018 5:53:54pm
	Author:  Ben

  ==============================================================================
*/

#include "BlockVizPanel.h"


BlockViz::BlockViz(const String & contentName) :
	ShapeShifterContentComponent(contentName),
	currentBlock(nullptr),
	blockRef(nullptr)
{
	vizProp.resolution->setValue(32);
	vizProp.shape->setValueWithData(Prop::CLUB);

	idUI = vizProp.id->createStepper();
	resolutionUI = vizProp.resolution->createStepper();
	shapeUI = vizProp.shape->createUI();

	addAndMakeVisible(idUI);
	addAndMakeVisible(resolutionUI);
	addAndMakeVisible(shapeUI);

	
	InspectableSelectionManager::mainSelectionManager->addSelectionListener(this);
}

BlockViz::~BlockViz()
{
	setCurrentBlock(nullptr);
	if(InspectableSelectionManager::mainSelectionManager != nullptr) InspectableSelectionManager::mainSelectionManager->removeSelectionListener(this);
}

void BlockViz::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	Rectangle<int> hr = r.removeFromTop(16);

	idUI->setBounds(hr.removeFromLeft(100));
	hr.removeFromLeft(2);
	resolutionUI->setBounds(hr.removeFromLeft(100));
	hr.removeFromLeft(2);
	shapeUI->setBounds(hr);

	r.removeFromTop(2);
	if(propViz != nullptr) propViz->setBounds(r);
}

void BlockViz::setCurrentBlock(LightBlock * b)
{
	if (b == currentBlock) return;
	

	if (currentBlock != nullptr)
	{
		if (currentBlock == vizBlock) vizBlock = nullptr;
		currentBlock = nullptr;
		if(blockRef != nullptr && !blockRef.wasObjectDeleted()) blockRef->removeInspectableListener(this);
		blockRef = nullptr;

		if(propViz != nullptr) removeChildComponent(propViz);
		propViz = nullptr;
		
	}

	currentBlock = b;
	blockRef = b;

	if (currentBlock != nullptr)
	{
		if (currentBlock != vizBlock.get()) blockRef->addInspectableListener(this);
		

		resolutionUI->setVisible(currentBlock->prop == &vizProp);
		idUI->setVisible(currentBlock->prop == &vizProp);
		shapeUI->setVisible(currentBlock->prop == &vizProp);

		propViz = new PropViz(currentBlock->prop);
		addAndMakeVisible(propViz);
		resized();
		
	}

	repaint();
}

void BlockViz::inspectableDestroyed(Inspectable * i)
{
	if(blockRef.wasObjectDeleted()) setCurrentBlock(nullptr);
}

void BlockViz::inspectablesSelectionChanged()
{
	LightBlockModel * m = InspectableSelectionManager::mainSelectionManager->getInspectableAs<LightBlockModel>();
	if (m != nullptr)
	{
		vizBlock = new LightBlock(m, &vizProp);
		setCurrentBlock(vizBlock);
		return;
	}

	Prop * p = InspectableSelectionManager::mainSelectionManager->getInspectableAs<Prop>();
	if (p != nullptr)
	{
		setCurrentBlock(p->currentBlock);
	}
}