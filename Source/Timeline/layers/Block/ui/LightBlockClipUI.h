/*
  ==============================================================================

    LightBlockClipUI.h
    Created: 17 Apr 2018 7:20:43pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockClip.h"

class LightBlockClipUI :
	public BaseItemUI<LightBlockClip>
{
public:
	LightBlockClipUI(LightBlockClip * clip);
	~LightBlockClipUI();

	//interaction
	float timeAtMouseDown;
	int posAtMouseDown;

	void paint(Graphics &g) override;
	void resizedInternalHeader(Rectangle<int> &r) override;

	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;

	void buttonClicked(Button * b) override;

	void controllableFeedbackUpdateInternal(Controllable *) override;

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
