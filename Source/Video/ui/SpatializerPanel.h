/*
  ==============================================================================

    SpatializerPanel.h
    Created: 23 Apr 2018 9:42:51pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../Spatializer.h"
#include "SpatItemUI.h"

class SpatializerPanel :
	public BaseManagerShapeShifterUI<Spatializer, SpatItem, SpatItemUI>,
	public ContainerAsyncListener,
	public Timer
{ 
public:
	SpatializerPanel(const String &name);
	~SpatializerPanel();

	Image bgImage;

	void paint(Graphics &g) override;
	void resized() override;

	SpatItemUI * createUIForItem(SpatItem * i) override;

	Point<float> getPositionForRelative(Point<float> relPos);
	Point<float> getRelativeForPosition(Point<float> absolutePos);

	void newMessage(const ContainerAsyncEvent &e) override;

	void timerCallback() override;

	static SpatializerPanel * create(const String &name) { return new SpatializerPanel(name); }
};