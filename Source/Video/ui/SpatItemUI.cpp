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

SpatItemUI::SpatItemUI(SpatItem * i, Spatializer * spat, SpatLayoutView * panel) :
	BaseItemMinimalUI(i),
	spat(spat),
	panel(panel),
    lockBounds(false),
	startHandle(true),
	endHandle(false)
{
	addAndMakeVisible(&startHandle);
	addAndMakeVisible(&endHandle);

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
	Point<float> startHandlePos = startHandle.getBounds().getCentre().toFloat();
	Point<float> endHandlePos = endHandle.getBounds().getCentre().toFloat();

	Colour c = item->isSelected ? Colours::yellow : (isMouseOver() ? HIGHLIGHT_COLOR : Colours::white);
	g.setColour(c.withAlpha(.4f));
	
	if (spat->showHandles->boolValue())
	{
		Prop::Shape s = item->shape->getValueDataAsEnum<Prop::Shape>();
		switch (s)
		{
		case Prop::Shape::CLUB:
			g.drawLine(Line<float>(startHandlePos, endHandlePos), item->isSelected ? 2 : 1);
			break;

		case Prop::Shape::BALL:
			break;
		case Prop::Shape::POI:
			break;
		case Prop::Shape::HOOP:
		{
			Rectangle<float> hr;

			Point<float> startPos = item->startPos->getPoint();
			Point<float> endPos = item->endPos->getPoint(); 
			float distP = endPos.getDistanceFrom(startPos);
			Point<float> relDistP = panel->getPositionForRelative(Point<float>(distP, distP)) *2;
			hr.setSize(relDistP.x, relDistP.y);
			hr.setCentre(startHandlePos);
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
		Point<float> startPos = item->startPos->getPoint();
		for (int i = 0; i < numPoints; i++)
		{
			g.setColour(item->colors[i]);
			g.fillEllipse(pr.withCentre(startHandlePos + panel->getPositionForRelative(item->points[i] - startPos)));
		}
	}
	
}

void SpatItemUI::resized()
{
	startHandle.setBounds(startHandle.getLocalBounds().withCentre(getLocalPoint(panel, panel->getPositionForRelative(item->startPos->getPoint())).toInt()));
	endHandle.setBounds(endHandle.getLocalBounds().withCentre(getLocalPoint(panel, panel->getPositionForRelative(item->endPos->getPoint())).toInt()));
}

void SpatItemUI::updateBounds()
{
	if (lockBounds) return;

	Prop::Shape s = item->shape->getValueDataAsEnum<Prop::Shape>();
	Rectangle<int> r;
		
	Point<float> p1 = panel->getPositionForRelative(item->startPos->getPoint());
	Point<float> p2 = panel->getPositionForRelative(item->endPos->getPoint());

	switch (s)
	{

	case Prop::Shape::HOOP:
	{
		float margin = 8;
		Point<float> startPos = item->startPos->getPoint();
		Point<float> endPos = item->endPos->getPoint();
		float distP = endPos.getDistanceFrom(startPos);
		Point<float> relDistP = panel->getPositionForRelative(Point<float>(distP, distP)) * 2;
		r.setSize(relDistP.x + margin, relDistP.y + margin);
		r.setCentre(p1.x, p1.y);

	}
	break;

	default:
	{
		Point<int> points[2];
		points[0] = p1.toInt();
		points[1] = p2.toInt();
		r = Rectangle<int>::findAreaContainingPoints(points, 2).expanded(10);
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
		startHandle.posAtDown = item->startPos->getPoint();
		endHandle.posAtDown = item->endPos->getPoint();
	}
}

void SpatItemUI::mouseDrag(const MouseEvent & e)
{
	if (e.eventComponent == &startHandle) item->startPos->setPoint(panel->getRelativeForPosition(e.getEventRelativeTo(panel).getPosition().toFloat()));
	else if (e.eventComponent == &endHandle) item->endPos->setPoint(panel->getRelativeForPosition(e.getEventRelativeTo(panel).getPosition().toFloat()));
	else if (e.eventComponent == this)
	{
		lockBounds = true;
		item->startPos->setPoint(startHandle.posAtDown + panel->getRelativeForPosition(e.getOffsetFromDragStart().toFloat()));
		item->endPos->setPoint(endHandle.posAtDown + panel->getRelativeForPosition(e.getOffsetFromDragStart().toFloat()));
		lockBounds = false;
		updateBounds();
	}
}

void SpatItemUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	if (c == item->shape)
	{
		setPaintingIsUnclipped(item->shape->getValueDataAsEnum<Prop::Shape>() == Prop::Shape::HOOP);
		repaint();
	} else if (c == item->startPos || c == item->endPos)
	{
		updateBounds();
	}
	else if (c == spat->showHandles)
	{
		startHandle.setVisible(spat->showHandles->boolValue());
		endHandle.setVisible(spat->showHandles->boolValue());
		repaint();
	}
}



// SPAT

SpatItemUI::Handle::Handle(bool isStart) :
	isStart(isStart)
{
	setSize(20, 20);
}

void SpatItemUI::Handle::paint(Graphics & g)
{
	g.setColour(isMouseOver() ? HIGHLIGHT_COLOR : (isStart ? GREEN_COLOR : BLUE_COLOR));
	Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(10, 10);
	if (isStart) g.fillRect(r);
	else g.fillEllipse(r.toFloat());
}
