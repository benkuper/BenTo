/*
  ==============================================================================

    LightBlockModelUI.h
    Created: 10 Apr 2018 8:10:04pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockModelUI :
	public BaseItemMinimalUI<LightBlockModel>,
	public LightBlockModel::AsyncListener
{
public:
	LightBlockModelUI(LightBlockModel * model);
	~LightBlockModelUI();

	Image modelImage;
	TimelineBlock* timelineBlock;

	//static const Identifier dragAndDropID;

	void paint(Graphics &g) override;

	virtual void updateThumbnail();

	virtual void editBlock() {} //to be overriden by child classes

	void newMessage(const LightBlockModel::ModelEvent &e) override;

	void controllableFeedbackUpdateInternal(Controllable* c) override;

	void mouseDown(const MouseEvent &e) override;
	void mouseDoubleClick(const MouseEvent &e) override;
};

