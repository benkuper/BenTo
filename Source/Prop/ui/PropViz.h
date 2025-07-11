/*
  ==============================================================================

	PropViz.h
	Created: 11 Apr 2018 10:17:51pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class PropViz :
	public Component,
	public Prop::AsyncListener
{
public:
	PropViz(Prop* prop);
	~PropViz();

	Prop* prop;
	WeakReference<Inspectable> propRef;
	BoolParameter* imuRef;

	bool shouldRepaint;
	bool updateDrawing;

	void paint(Graphics& g) override;
	void newMessage(const Prop::PropEvent& e) override;

	void mouseDown(const MouseEvent& e) override;

	void handleRepaint();
};

class VizTimer :
	public Timer
{
public:
	juce_DeclareSingleton(VizTimer, true);

	VizTimer();
	~VizTimer() {}

	Array<PropViz*, CriticalSection> vizArray;

	void registerViz(PropViz* viz);
	void unregisterViz(PropViz* viz);

	void timerCallback() override;
};