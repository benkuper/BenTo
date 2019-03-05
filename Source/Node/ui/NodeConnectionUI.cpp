/*
  ==============================================================================

	NodeConnectionUI.cpp
	Created: 13 Apr 2018 11:26:29pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeConnectionUI.h"
#include "LightBlock/model/ui/LightBlockModelUI.h"
#include "Node/nodes/model/ModelNode.h"

NodeConnectionUI::NodeConnectionUI(NodeConnection * nc) :
	BaseItemMinimalUI(nc),
	sourceConnector(nullptr),
	destConnector(nullptr),
	isDraggingModel(false)
{
	autoDrawContourWhenSelected = false;
	setRepaintsOnMouseActivity(true);
}

NodeConnectionUI::~NodeConnectionUI()
{
	if (sourceConnector != nullptr)
	{
		sourceConnector->removeComponentListener(this);
		sourceConnector->getParentComponent()->removeComponentListener(this);
	}

	if (destConnector != nullptr)
	{
		destConnector->removeComponentListener(this);
		destConnector->getParentComponent()->removeComponentListener(this);
	}
}

void NodeConnectionUI::paint(Graphics & g)
{
	if (sourceConnector == nullptr || destConnector == nullptr) return;

	Colour c = item->isSelected?HIGHLIGHT_COLOR:NodeConnectionUI::getColorForType(sourceConnector->slot->type);
	if (isDraggingModel) c = BLUE_COLOR;
	if (isMouseOver()) c = c.brighter();
	g.setColour(c);
	g.strokePath(path, PathStrokeType(isDraggingModel?6:3));
}

void NodeConnectionUI::resized()
{
	if (sourceConnector == nullptr || destConnector == nullptr) return;
}

bool NodeConnectionUI::hitTest(int x, int y)
{
	return hitPath.contains((float)x, (float)y);
}

void NodeConnectionUI::buildPath()
{
	Point<float> sp = getLocalPoint(sourceConnector, sourceConnector->getLocalBounds().getCentre().toFloat());
	Point<float> dp = getLocalPoint(destConnector, destConnector->getLocalBounds().getCentre().toFloat());

	path.clear(); 
	path.startNewSubPath(sp);
	path.cubicTo(sp.translated(60, 0), dp.translated(-60, 0), dp);

	buildHitPath();
}

void NodeConnectionUI::buildHitPath()
{

	hitPath.clear();
	Array<Point<float>> firstPoints;
	Array<Point<float>> secondPoints;
	
	const int numPoints = 10;
	const int margin = 5;

	Array<Point<float>> points;
	for (int i = 0; i < numPoints; i++)
	{
		points.add(path.getPointAlongPath(path.getLength()*i / numPoints));
	}

	for (int i = 0; i < numPoints; i++)
	{
		Point<float> tp;
		Point<float> sp;

		if (i == 0 || i == numPoints - 1)
		{
			tp = points[i].translated(0, -margin);
			sp = points[i].translated(0, margin);
		} else
		{
			float angle1 = points[i].getAngleToPoint(points[i - 1]);
			float angle2 = points[i].getAngleToPoint(points[i + 1]);

			if (angle1 < 0) angle1 += float_Pi * 2;

			if (angle2 < 0) angle2 += float_Pi * 2;

			float angle = (angle1 + angle2) / 2.f;

			if (angle1 < angle2) angle += float_Pi;

			//            DBG("Point " << i << ", angle : " << angle << " >>  " << String(angle1>angle2));

			tp = points[i].getPointOnCircumference(margin, angle + float_Pi);
			sp = points[i].getPointOnCircumference(margin, angle);
		}

		firstPoints.add(tp);
		secondPoints.insert(0, sp);
	}

	hitPath.startNewSubPath(firstPoints[0]);

	for (int i = 1; i < firstPoints.size(); i++) hitPath.lineTo(firstPoints[i]);

	for (int i = 0; i < secondPoints.size(); i++) hitPath.lineTo(secondPoints[i]);

	hitPath.closeSubPath();
}

void NodeConnectionUI::setConnectors(NodeViewUI::Connector * source, NodeViewUI::Connector * dest)
{
	sourceConnector = source;
	destConnector = dest;
	sourceConnector->getParentComponent()->addComponentListener(this);
	destConnector->getParentComponent()->addComponentListener(this);
	sourceConnector->addComponentListener(this);
	destConnector->addComponentListener(this);
	updateBounds();
}

void NodeConnectionUI::updateBounds()
{
	if (sourceConnector == nullptr || destConnector == nullptr) return;

	Point<int> points[2];
	points[0] = getParentComponent()->getLocalPoint(sourceConnector, sourceConnector->getLocalBounds().getCentre());
	points[1] = getParentComponent()->getLocalPoint(destConnector, destConnector->getLocalBounds().getCentre());
	Rectangle<int> r = Rectangle<int>::findAreaContainingPoints(points, 2).expanded(16);
	setBounds(r);
	buildPath();
	resized();
}

void NodeConnectionUI::componentMovedOrResized(Component & c, bool, bool)
{
	updateBounds();
}

void NodeConnectionUI::componentBeingDeleted(Component & c)
{
	if (&c == sourceConnector) sourceConnector = nullptr;
	else if (&c == destConnector) destConnector = nullptr;
}



bool NodeConnectionUI::isInterestedInDragSource(const SourceDetails & source)
{
	return source.description.getProperty("type", "") == "NodeTool";
}

void NodeConnectionUI::itemDragEnter(const SourceDetails & source)
{
	isDraggingModel = true;
	repaint();
}

void NodeConnectionUI::itemDragExit(const SourceDetails & source)
{
	isDraggingModel = false;
	repaint();
}


void NodeConnectionUI::itemDropped(const SourceDetails & source)
{
	String nodeType = source.description.getProperty("nodeType", "");
	if (nodeType.isEmpty())
	{
		DBG("No nodeType");
		return;
	}

	item->insertNode(nodeType);
	isDraggingModel = false;
	repaint();
}



Colour NodeConnectionUI::getColorForType(ConnectionType t) {
	switch (t)
	{
	case ConnectionType::ColorBlock:
		return Colours::cornflowerblue;

	case ConnectionType::Number:
		return Colours::limegreen;

	case ConnectionType::Color:
		return Colours::hotpink;
	}

	return Colours::black;
}

String NodeConnectionUI::getStringForConnectionType(ConnectionType t) {
	switch (t)
	{
	case ConnectionType::ColorBlock: return "Colors";

	case ConnectionType::Number: return "Number";

	case ConnectionType::Color: return "Single Color";
	}

	return "Unknown";
}
