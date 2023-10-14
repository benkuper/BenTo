/*
  ==============================================================================

	BlockVizPanel.cpp
	Created: 11 Apr 2018 5:53:54pm
	Author:  Ben

  ==============================================================================
*/

#include "Common/CommonIncludes.h"
#include "Prop/PropIncludes.h"

BlockViz::BlockViz(const String& contentName) :
	ShapeShifterContentComponent(contentName)
{
	vizProp.reset(new Prop());
	vizProp->resolution->setValue(32);
	vizProp->shape->setValueWithData(Prop::CLUB);

	idUI.reset(vizProp->globalID->createStepper());
	resolutionUI.reset(vizProp->resolution->createStepper());
	shapeUI.reset(vizProp->shape->createUI());

	addAndMakeVisible(idUI.get());
	addAndMakeVisible(resolutionUI.get());
	addAndMakeVisible(shapeUI.get());


	InspectableSelectionManager::mainSelectionManager->addAsyncSelectionManagerListener(this);
}

BlockViz::~BlockViz()
{
	if (InspectableSelectionManager::mainSelectionManager != nullptr) InspectableSelectionManager::mainSelectionManager->removeAsyncSelectionManagerListener(this);
}

void BlockViz::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	Rectangle<int> hr = r.removeFromTop(16);
	r.removeFromTop(2);
	Rectangle<int> hr2 = r.removeFromTop(16);

	shapeUI->setBounds(hr);
	idUI->setBounds(hr2.removeFromLeft(100));
	hr2.removeFromLeft(2);
	resolutionUI->setBounds(hr2.removeFromLeft(100));

	r.removeFromTop(2);
	if (propViz != nullptr) propViz->setBounds(r);
}

void BlockViz::newMessage(const InspectableSelectionManager::SelectionEvent& e)
{
	if (e.type == InspectableSelectionManager::SelectionEvent::SELECTION_CHANGED)
	{
		LightBlockColorProvider* m = InspectableSelectionManager::mainSelectionManager->getInspectableAs<LightBlockColorProvider>();
		if (m != nullptr)
		{
			idUI->setVisible(true);
			resolutionUI->setVisible(true);
			shapeUI->setVisible(true);
			vizProp->setBlockFromProvider(m);
			if (propViz != nullptr) removeChildComponent(propViz.get());
			propViz.reset(new PropViz(vizProp.get()));
			addAndMakeVisible(propViz.get());
			resized();
			return;
		}

		Prop* p = InspectableSelectionManager::mainSelectionManager->getInspectableAs<Prop>();
		if (p != nullptr)
		{
			idUI->setVisible(false);
			resolutionUI->setVisible(false);
			shapeUI->setVisible(false);
			vizProp->setBlockFromProvider(m);
			if (propViz != nullptr) removeChildComponent(propViz.get());
			propViz.reset(new PropViz(p));
			addAndMakeVisible(propViz.get());
			resized();
			return;
		}
	}

}