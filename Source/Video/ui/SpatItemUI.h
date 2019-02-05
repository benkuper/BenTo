/*
  ==============================================================================

    SpatItemUI.h
    Created: 23 Apr 2018 9:43:26pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../SpatItem.h"
class Spatializer;
class SpatLayoutView;

class SpatItemUI :
	public BaseItemMinimalUI<SpatItem>
{
public:
	SpatItemUI(SpatItem * i, Spatializer * spat = nullptr, SpatLayoutView * panel = nullptr);
	~SpatItemUI();

	Spatializer * spat;
	SpatLayoutView * panel; //for handle positionning in reference

	bool lockBounds;

	void paint(Graphics &g) override;
	void resized() override;

	void updateBounds();
	

	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;

	void controllableFeedbackUpdateInternal(Controllable * c) override;

	class Handle :
		public Component
	{
	public:
		Handle(bool isStart);
		~Handle() {}

		Point<float> posAtDown;

		bool isStart;
		void paint(Graphics &g) override;
	};

	Handle startHandle;
	Handle endHandle;

};