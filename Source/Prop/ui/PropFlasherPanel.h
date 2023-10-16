/*
  ==============================================================================

	PropFlasherPanel.h
	Created: 1 Jan 2023 7:36:22pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class PropFlasherPanel :
	public ShapeShifterContentComponent,
	public SerialManager::SerialManagerListener,
	public Parameter::AsyncListener
{
public:
	PropFlasherPanel();
	~PropFlasherPanel();

	Rectangle<int> propInfosRect;
	Rectangle<int> controlRect;
	Rectangle<int> fwRect;
	Rectangle<int> wifiRect;
	Rectangle<int> flashRect;
	Rectangle<int> uploadRect;

	Array<SerialDeviceInfo*> infos;

	std::unique_ptr<BoolToggleUI> filterKnownDevicesUI;

	std::unique_ptr<EnumParameterUI> firmwareToUploadUI;
	std::unique_ptr<StringParameterFileUI> firmwareCustomFileUI;
	std::unique_ptr<BoolToggleUI> setWifiAfterFlashUI;
	std::unique_ptr<StringParameterUI> wifiSSIDUI;
	std::unique_ptr<StringParameterUI> wifiPassUI;

	std::unique_ptr<TriggerButtonUI> flashAllUI;
	std::unique_ptr<FloatSliderUI> progressUI;

	std::unique_ptr<StringParameterFileUI> serverFolder;
	std::unique_ptr<TriggerButtonUI> uploadServerFiles;


	virtual void portAdded(SerialDeviceInfo* info) override;
	virtual void portRemoved(SerialDeviceInfo* info) override;

	virtual void newMessage(const Parameter::ParameterEvent& e) override;

	void updateInfos();

	void paint(Graphics& g) override;
	void resized() override;


	static String getTypeStringStatic() { return "Firmware Uploader"; }
	static PropFlasherPanel* create(const String& name) { return new PropFlasherPanel(); }
};