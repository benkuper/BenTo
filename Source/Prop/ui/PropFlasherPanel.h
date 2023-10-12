/*
  ==============================================================================

	PropFlasherPanel.h
	Created: 1 Jan 2023 7:36:22pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class PropFlasherPanel :
	public ShapeShifterContentComponent
{
public:
	PropFlasherPanel();
	~PropFlasherPanel();

	Rectangle<int> propInfosRect;
	Rectangle<int> controlRect;
	Rectangle<int> fwRect;
	Rectangle<int> wifiRect;
	Rectangle<int> flashRect;

	std::unique_ptr<EnumParameterUI> firmwareToUploadUI;
	std::unique_ptr<StringParameterFileUI> firmwareCustomFileUI;
	std::unique_ptr<BoolToggleUI> setWifiAfterFlashUI;
	std::unique_ptr<StringParameterUI> wifiSSIDUI;
	std::unique_ptr<StringParameterUI> wifiPassUI;

	std::unique_ptr<TriggerButtonUI> flashAllUI;

	std::unique_ptr<FloatSliderUI> progressUI;




	void paint(Graphics& g) override;
	void resized() override;


	static PropFlasherPanel* create(const String& name) { return new PropFlasherPanel(); }
};