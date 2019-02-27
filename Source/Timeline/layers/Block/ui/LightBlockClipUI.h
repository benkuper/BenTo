/*
  ==============================================================================

    LightBlockClipUI.h
    Created: 17 Apr 2018 7:20:43pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockClip.h"
#include "Prop/Prop.h"

class LightBlockFadeHandle :
	public Component
{
public:
	LightBlockFadeHandle(const Image &image);
	~LightBlockFadeHandle() {}

	Image img;

	void paint(Graphics &g) override;
};

class LightBlockClipUI :
	public LayerBlockUI,
	public Thread //Threaded preview generation,
{
public:
	LightBlockClipUI(LightBlockClip * clip);
	~LightBlockClipUI();

	LightBlockClip * clip;

	ScopedPointer<AutomationUI> automationUI;

	LightBlockFadeHandle fadeInHandle;
	LightBlockFadeHandle fadeOutHandle;

	//
	float fadeValueAtMouseDown;

	Prop previewProp;
	bool imageIsReady;
	Image previewImage;

	void paint(Graphics &g) override;
	void resizedBlockInternal() override;

	void generatePreview();

	void setTargetAutomation(ParameterAutomation * a);

	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;

	Rectangle<int> getDragBounds() override;

	void controllableFeedbackUpdateInternal(Controllable *) override;

	void run() override;
};
