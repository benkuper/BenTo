/*
  ==============================================================================

	BlockVizPanel.cpp
	Created: 11 Apr 2018 5:53:54pm
	Author:  Ben

  ==============================================================================
*/

#include "BlockVizPanel.h"


BlockViz::BlockViz(const String & contentName) :
	ShapeShifterContentComponent(contentName)
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


void BlockViz::inspectablesSelectionChanged()
{
	LightBlockColorProvider * m = InspectableSelectionManager::mainSelectionManager->getInspectableAs<LightBlockColorProvider>();
	if (m != nullptr)
	{
		idUI->setVisible(true);
		resolutionUI->setVisible(true);
		shapeUI->setVisible(true);
		vizProp.setBlockFromProvider(m);
		if (propViz != nullptr) removeChildComponent(propViz);
		propViz = new PropViz(&vizProp);
		addAndMakeVisible(propViz);
		resized();
		return;
	}

	Prop * p = InspectableSelectionManager::mainSelectionManager->getInspectableAs<Prop>();
	if (p != nullptr)
	{
		idUI->setVisible(false);
		resolutionUI->setVisible(false);
		shapeUI->setVisible(false);
		vizProp.setBlockFromProvider(m);
		if (propViz != nullptr) removeChildComponent(propViz);
		propViz = new PropViz(p);
		addAndMakeVisible(propViz);
		resized();
		return;
	}
}