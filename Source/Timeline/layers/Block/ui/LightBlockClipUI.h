/*
  ==============================================================================

    LightBlockClipUI.h
    Created: 17 Apr 2018 7:20:43pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockClip.h"

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
	public BaseItemMinimalUI<LightBlockClip>,
	public Thread //Threaded preview generation,
{
public:
	LightBlockClipUI(LightBlockClip * clip);
	~LightBlockClipUI();


	ScopedPointer<AutomationUI> automationUI;

	LightBlockFadeHandle fadeInHandle;
	LightBlockFadeHandle fadeOutHandle;

	//
	bool imageIsReady;
	Image previewImage;

	//interaction
	float timeAtMouseDown;
	int posAtMouseDown;

	void paint(Graphics &g) override;
	void resized() override;
	//void resizedInternalHeader(Rectangle<int> &r) override;

	void generatePreview();

	void setTargetAutomation(Automation * a);

	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;

	//void buttonClicked(Button * b) override;

	void controllableFeedbackUpdateInternal(Controllable *) override;

	void run() override;

	class ClipUIListener
	{
	public:
		virtual ~ClipUIListener() {}
		virtual void clipUITimeChanged(LightBlockClipUI *) {}
		virtual void clipUIDragged(LightBlockClipUI *, const MouseEvent &) {}
	};

	ListenerList<ClipUIListener> clipUIListeners;
	void addClipUIListener(ClipUIListener* newListener) { clipUIListeners.add(newListener); }
	void removeClipUIListener(ClipUIListener* listener) { clipUIListeners.remove(listener); }

};
