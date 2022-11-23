/*
  ==============================================================================

    SpatItem.cpp
    Created: 23 Apr 2018 9:43:20pm
    Author:  Ben

  ==============================================================================
*/

SpatItem::SpatItem() :
	BaseItem("Spat Item"),
	handlesCC("Handles"),
	isUpdatingHandles(false)
{

	isDefault = addBoolParameter("Default", "If checked and no item with the requested id is found, will fall back to this one", false);
	uiColor = addColorParameter("UI Color", "Color to differentiate in UI", NORMAL_COLOR);

	filterManager.reset(new PropTargetFilterManager());
	addChildControllableContainer(filterManager.get());

	shape = addEnumParameter("Shape", "The shape of the prop");
	shape->addOption("Club", Prop::Shape::CLUB)->addOption("Ball", Prop::Shape::BALL)->addOption("Poi", Prop::Shape::POI)->addOption("Hoop", Prop::Shape::HOOP)->addOption("Custom", Prop::Shape::CUSTOM);

	startIndex = addIntParameter("Start Index", "Index of the first pixel to use for this item", 1, 1);
	resolution = addIntParameter("Resolution", "Number of controllable colors in the prop", 32, 1);
	

	updateHandles();
	
	addChildControllableContainer(&handlesCC);
}

SpatItem::~SpatItem()
{

}


void SpatItem::updateHandles()
{
	isUpdatingHandles = true;
	handlesCC.clear();
	handles.clear();

	Prop::Shape s = shape->getValueDataAsEnum<Prop::Shape>();

	switch (s)
	{
	case Prop::Shape::CLUB:
	{
		handles.add(handlesCC.addPoint2DParameter("Start", "Position"));
		handles.add(handlesCC.addPoint2DParameter("End", "Position"));
		handles[0]->setPoint(.4f, .5f);
		handles[1]->setPoint(.6f, .5f);
	}
	break;

	case Prop::Shape::BALL:
	case Prop::Shape::POI:
		handles.add(handlesCC.addPoint2DParameter("Position", "Position"));
		handles[0]->setPoint(.5f, .5f);
		break;

	case Prop::Shape::HOOP:
	{
		handles.add(handlesCC.addPoint2DParameter("Center", "Position"));
		handles.add(handlesCC.addPoint2DParameter("Radius", "Position"));
		handles[0]->setPoint(.5f, .5f);
		handles[1]->setPoint(.6f, .5f);
	}
	break;

	case Prop::Shape::CUSTOM:
		updateCustomHandles();
		break;

	default:
		break;
	}

	isUpdatingHandles = false;

	updatePoints();

}

void SpatItem::updateCustomHandles()
{
	isUpdatingHandles = true;

	while (handles.size() > resolution->intValue())
	{
		handlesCC.removeControllable(handles[handles.size() - 1]);
		handles.removeLast();
	}

	while (handles.size() < resolution->intValue())
	{
		float p = handles.size() *1.0f / resolution->intValue();
		handles.add(handlesCC.addPoint2DParameter("Position " + String(handles.size() + 1), "Position"));
		handles[handles.size() - 1]->setPoint(.1f+.9f*p, .5f);
	}

	isUpdatingHandles = false;
}

void SpatItem::updatePoints()
{
	if (isUpdatingHandles) return;

	points.clear();

	int numPoints = resolution->intValue();

	switch (shape->getValueDataAsEnum<Prop::Shape>())
	{
	case Prop::Shape::CLUB:
	{

		Point<float> startPoint = handles[0]->getPoint();
		Point<float> endPoint = handles[1]->getPoint();
		for (int i = 0; i < numPoints; i++)
		{
			points.add(startPoint + (endPoint - startPoint) * (i * 1.0f / jmax(numPoints - 1, 1)));
		}
	}
	break;

	case Prop::Shape::POI:
	case Prop::Shape::BALL:
	{

		Point<float> center = handles[0]->getPoint();
		for (int i = 0; i < numPoints; i++)
		{
			points.add(center);
		}
	}
	break;

	case Prop::Shape::BOX:
	{

	}
	break;
	
	case Prop::Shape::HOOP:
	{
		Point<float> centerP = handles[0]->getPoint();
		Point<float> radiusP = handles[1]->getPoint();
		
		float angle = MathConstants<float>::pi * 2 / numPoints;
		float startAngle = centerP.getAngleToPoint(radiusP);
		float radius = centerP.getDistanceFrom(radiusP);
		for (int i = 0; i < numPoints; i++)
		{
			float tAngle = startAngle + angle * i;
			points.add(centerP + Point<float>(cosf(tAngle) * radius, sinf(tAngle) * radius));
		}
	}
	break;

	case Prop::Shape::CUSTOM:
	{
		for (int i = 0; i < numPoints; i++)
		{
			points.add(handles[i]->getPoint());
		}
	}
	break;

            
    default:
        break;

	
	}

	colors.resize(points.size());
}

void SpatItem::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == shape)
	{
		updateHandles();
	}
	else if (p == resolution)
	{
		if (shape->getValueDataAsEnum<Prop::Shape>() == Prop::CUSTOM)
		{
			updateCustomHandles();
			updatePoints();
		}
	}
}

void SpatItem::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	if (cc == &handlesCC) updatePoints();

}

var SpatItem::getJSONData()
{
	var data = BaseItem::getJSONData();
	data.getDynamicObject()->setProperty("filters", filterManager->getJSONData());
	data.getDynamicObject()->setProperty("handles", handlesCC.getJSONData());
	return data;
}

void SpatItem::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	filterManager->loadJSONData(data.getProperty("filters", var()));
	handlesCC.loadJSONData(data.getProperty("handles", var()));
}
