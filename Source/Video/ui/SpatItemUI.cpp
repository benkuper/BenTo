/*
  ==============================================================================

	SpatItemUI.cpp
	Created: 23 Apr 2018 9:43:26pm
	Author:  Ben

  ==============================================================================
*/

#include "SpatItemUI.h"
#include "SpatializerPanel.h"
#include "Video/Spatializer.h"
#include "Prop/Prop.h"

SpatItemUI::SpatItemUI(SpatItem* i, Spatializer* spat, SpatLayoutView* panel) :
	BaseItemMinimalUI(i),
	spat(spat),
	panel(panel),
	lockBounds(false)
{
	updateHandles();

	autoDrawContourWhenSelected = false;
	setRepaintsOnMouseActivity(true);

	spat->addAsyncContainerListener(this);
}


SpatItemUI::~SpatItemUI()
{
	if (spat != nullptr) spat->removeAsyncContainerListener(this);
}

void SpatItemUI::paint(Graphics & g)
{
	if (Engine::mainEngine->isClearing) return;
	
	Colour c = item->isSelected ? Colours::yellow : (isMouseOver() ? HIGHLIGHT_COLOR : Colours::white);
	g.setColour(c.withAlpha(.4f));
	
	if (spat->showHandles->boolValue())
	{
		Prop::Shape s = item->shape->getValueDataAsEnum<Prop::Shape>();
		switch (s)
		{
		case Prop::Shape::CLUB:
			g.drawLine(Line<float>(handles[0]->getBounds().getCentre().toFloat(), handles[1]->getBounds().getCentre().toFloat()), item->isSelected ? 2 : 1);
			break;

		case Prop::Shape::BALL:
			break;
		case Prop::Shape::POI:
			break;
		case Prop::Shape::HOOP:
		{
			Rectangle<float> hr;

			Point<float> startPos = item->handles[0]->getPoint();
			Point<float> endPos = item->handles[0]->getPoint(); 
			float distP = endPos.getDistanceFrom(startPos);
			Point<float> relDistP = panel->getPositionForRelative(Point<float>(distP, distP)) *2;
			hr.setSize(relDistP.x, relDistP.y);
			hr.setCentre(handles[0]->getBounds().getCentre().toFloat());
			g.drawEllipse(hr, item->isSelected ? 2 : 1);
		}
		break;
                
            default:
                break;
		}
	}

	if (spat->showPixels->boolValue())
	{
		int numPoints = item->points.size();
		Rectangle<float> pr(0, 0, 4, 4);
		for (int i = 0; i < numPoints; i++)
		{
			g.setColour(item->colors[i]);
			g.fillEllipse(pr.withCentre(panel->getPositionForRelative(item->points[i]) - getBoundsInParent().getTopLeft().toFloat()));
		}
	}
	
}

void SpatItemUI::resized()
{
	if (item->isUpdatingHandles || handles.size() != item->handles.size()) return;
	for(auto & h : handles) h->setBounds(h->getLocalBounds().withCentre(getLocalPoint(panel, panel->getPositionForRelative(item->handles[h->index]->getPoint())).toInt()));
}

void SpatItemUI::updateBounds()
{
	if (lockBounds) return;

	Prop::Shape s = item->shape->getValueDataAsEnum<Prop::Shape>();
	Rectangle<int> r;
		
	
	switch (s)
	{

	case Prop::Shape::HOOP:
	{
		Point<float> p1 = panel->getPositionForRelative(item->handles[0]->getPoint());
		Point<float> p2 = panel->getPositionForRelative(item->handles[1]->getPoint());
		
		float margin = 8;
		Point<float> startPos = item->handles[0]->getPoint();
		Point<float> endPos = item->handles[1]->getPoint();
		float distP = endPos.getDistanceFrom(startPos);
		Point<float> relDistP = panel->getPositionForRelative(Point<float>(distP, distP)) * 2;
		r.setSize(relDistP.x + margin, relDistP.y + margin);
		r.setCentre(p1.x, p1.y);

	}
	break;

	default:
	{
		Array<Point<int>> points;
		for (auto& h : item->handles) points.add(panel->getPositionForRelative(h->getPoint()).toInt());
		r = Rectangle<int>::findAreaContainingPoints(points.getRawDataPointer(), points.size()).expanded(10);
	}
	break;
	}

	setBounds(r);
}

void SpatItemUI::mouseDown(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseDown(e);
	if (e.eventComponent == this)
	{
		for (auto& h : handles) h->posAtDown = item->handles[h->index]->getPoint();
	}
}

void SpatItemUI::mouseDrag(const MouseEvent & e)
{
	if (Handle* th = dynamic_cast<Handle*>(e.eventComponent))
	{
		item->handles[th->index]->setPoint(panel->getRelativeForPosition(e.getEventRelativeTo(panel).getPosition().toFloat()));
		resized();
	}
	else if (e.eventComponent == this)
	{
		lockBounds = true;
		for (auto& h : handles) item->handles[h->index]->setPoint(h->posAtDown + panel->getRelativeForPosition(e.getOffsetFromDragStart().toFloat()));
		lockBounds = false;
		updateBounds();
		resized();
	}
}

void SpatItemUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	if (c == item->shape)
	{
		updateHandles();
		setPaintingIsUnclipped(item->shape->getValueDataAsEnum<Prop::Shape>() == Prop::Shape::HOOP);
		repaint();
	}
	else if (c == item->resolution)
	{
		if(item->shape->getValueDataAsEnum<Prop::Shape>() == Prop::Shape::CUSTOM) updateHandles();
	}
	else if (c->parentContainer == &item->handlesCC)
	{
		updateBounds();
	}
	else if (c == spat->showHandles)
	{
		for(auto & h : handles) h->setVisible(spat->showHandles->boolValue());
		repaint();
	}
}

void SpatItemUI::updateHandles()
{
	for (auto& h : handles) removeChildComponent(h);
	handles.clear();

	bool showLabel = item->shape->getValueDataAsEnum<Prop::Shape>() == Prop::CUSTOM;
	for (int i = 0; i < item->handles.size(); i++)
	{
		Handle* h = new Handle(i, showLabel);
		addAndMakeVisible(h);
		handles.add(h);
	}
}



// SPAT

SpatItemUI::Handle::Handle(int index, bool showLabel) :
	index(index),
	showLabel(showLabel)
{
	setSize(20, 20);
}

void SpatItemUI::Handle::paint(Graphics & g)
{
	int size = showLabel ? 16 : 10;
	Colour c = isMouseOver() ? HIGHLIGHT_COLOR : (index == 0 ? GREEN_COLOR : BLUE_COLOR);
	g.setColour(c);
	Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(size, size);
	if (index == 0) g.fillRect(r);
	else g.fillEllipse(r.toFloat());

	if (showLabel)
	{
		g.setColour(c.darker(.7f));
		g.setFont(r.getHeight() - 2);
		g.drawFittedText(String(index+1), r, Justification::centred, 1);
	}
}
