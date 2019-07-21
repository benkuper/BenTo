/*
  ==============================================================================

    LightBlockClipUI.h
    Created: 17 Apr 2018 7:20:43pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockClip.h"

class Prop;

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
	public Thread, //Threaded preview generation
	public Timer
{
public:
	LightBlockClipUI(LightBlockClip * clip);
	~LightBlockClipUI();

	LightBlockClip * clip;

	std::unique_ptr<Component> automationUI;

	LightBlockFadeHandle fadeInHandle;
	LightBlockFadeHandle fadeOutHandle;

	//
	float fadeValueAtMouseDown;

	std::unique_ptr<Prop> previewProp;
	bool shouldUpdateImage;
	bool imageIsReady;
	bool shouldRepaint;

	SpinLock imgLock;
	Image previewImage;


	bool isDraggingModel;

	void paint(Graphics &g) override;
	void paintOverChildren(Graphics &g) override;

	void resizedBlockInternal() override;

	void generatePreview();

	void setTargetAutomation(ParameterAutomation * a);

	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;

	Rectangle<int> getDragBounds() override;

	void controllableFeedbackUpdateInternal(Controllable *) override;

	void itemDropped(const SourceDetails &source) override;

	void timerCallback() override;

	void run() override;

};
