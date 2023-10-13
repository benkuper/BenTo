/*
  ==============================================================================

    BentoProp.h
    Created: 5 Mar 2019 11:01:56pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class BentoProp :
	public Prop,
	public SerialDevice::SerialDeviceListener
{
public:
	BentoProp(var params);
	virtual ~BentoProp();
	StringParameter * remoteHost;

	const int remotePort = 8888;
	DatagramSocket sender;
	OSCSender oscSender;

	SerialDeviceParameter* serialParam;
	SerialDevice* serialDevice;

	DMXArtNetDevice artnet;
	DMXUniverse universe;
	Array<uint8> data;

	virtual void clearItem() override;

	virtual void setSerialDevice(SerialDevice* d);

	void onContainerParameterChangedInternal(Parameter* p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *c) override;

	virtual void serialDataReceived(SerialDevice * d, const var &) override;
	virtual void portRemoved(SerialDevice* d) override;

	virtual void sendColorsToPropInternal() override;

	virtual void uploadPlaybackData(PlaybackData data) override;
	virtual void exportPlaybackData(PlaybackData data) override;
	virtual void uploadFile(File f) override;

	virtual void loadPlayback(StringRef fileName, bool autoPlay = false) override;
	virtual void playPlayback(float time = 0, bool loop = false) override;
	virtual void pausePlaybackPlaying() override;
	virtual void resumePlaybackPlaying() override {}
	virtual void seekPlaybackPlaying(float time) override;
	virtual void stopPlaybackPlaying() override;
	virtual void sendShowPropID(bool value) override;

	bool uploadProgressCallback(int bytesSent, int totalBytes);
	bool uploadMetaDataProgressCallback(int bytesSent, int totalBytes);

	virtual void sendYo();
	virtual void sendPingInternal() override;
	virtual void powerOffProp() override;
	virtual void restartProp() override;
	virtual void sendWiFiCredentials(String ssid, String pass);

	virtual void sendControlToPropInternal(String control, var value = var()) override;
	virtual void sendMessageToProp(const OSCMessage& m);


	static var sendMessageToPropFromScript(const var::NativeFunctionArgs& a);

	DECLARE_TYPE("Bento Prop");
};
