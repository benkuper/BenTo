/*
  ==============================================================================

	BentoProp.cpp
	Created: 5 Mar 2019 11:01:56pm
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"

BentoProp::BentoProp(var params) :
	Prop(params),
	serialDevice(nullptr),
	universe(0),
	resolutionRef(nullptr),
	brightnessRef(nullptr)
{
	updateRate = 100;
	useAlphaInPlaybackData = true;
	invertLedsInUI = true;

	remoteHost = connectionCC.addStringParameter("Network IP", "IP of the prop on the network", "");

	resolution->setDefaultValue(32);
	brightness = generalCC.addFloatParameter("Brightness", "Brightness of the prop", 1, 0, 1);

	artnet.inputCC->enabled->setValue(false);

	serialParam = new SerialDeviceParameter("USB Port", "For connecting props through USB", true);
	serialParam->setBaudrate(115200);

#if !JUCE_MAC
	if (params.hasProperty("vid")) serialParam->vidFilters.add((int)params.getProperty("vid", 0));
	if (params.hasProperty("pid")) serialParam->pidFilters.add((int)params.getProperty("pid", 0));
#endif

	scriptObject.getDynamicObject()->setMethod("send", &BentoProp::sendMessageToPropFromScript);

	connectionCC.addParameter(serialParam);

	oscSender.connect("127.0.0.1", 1024);

	data.resize(DMX_NUM_CHANNELS);
	data.fill(0);

	componentsCC.reset(new BentoComponentContainer(this));
	addChildControllableContainer(componentsCC.get(), false, controllableContainers.indexOf(scriptManager.get()));

	playbackAddress = "/leds/strip1/playbackLayer";
}

BentoProp::~BentoProp()
{

}

void BentoProp::clearItem()
{
	Prop::clearItem();
	setSerialDevice(nullptr);
}

void BentoProp::setSerialDevice(SerialDevice* d)
{
	if (serialDevice == d) return;

	if (serialDevice != nullptr)
	{
		serialDevice->removeSerialDeviceListener(this);
	}

	serialDevice = d;

	if (serialDevice != nullptr)
	{
		if (serialDevice->isOpen()) serialDevice->addSerialDeviceListener(this);
		else NLOGERROR(niceName, "Error opening port " << serialDevice->info->description);
	}
}

void BentoProp::onContainerParameterChangedInternal(Parameter* p)
{
	Prop::onContainerParameterChangedInternal(p);

	if (p == enabled)
	{
		sendMessageToProp(OSCMessage("/rgb/enabled", enabled->boolValue() ? 1 : 0));
	}
}

void BentoProp::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	Prop::onControllableFeedbackUpdateInternal(cc, c);
	if (c == playbackMode)
	{
		if (playbackMode->boolValue())
		{
			String filename = currentBlock != nullptr ? currentBlock->shortName : (playbackFileName->enabled ? playbackFileName->stringValue() : "");
			if (filename.isNotEmpty()) loadPlayback(filename);
		}
		else
		{
			OSCMessage m("/leds/mode");
			m.addString("stream");
			sendMessageToProp(m);
		}

	}
	else if (c == serialParam)
	{
		setSerialDevice(serialParam->getDevice());
	}
	else if (c == remoteHost)
	{
		//sendYo();
		artnet.remoteHost->setValue(remoteHost->stringValue());
	}
	else if (c == resolution)
	{
		if (resolutionRef != nullptr) resolutionRef->setValue(resolution->intValue());
	}
	else if (c == resolutionRef)
	{
		resolution->setValue(resolutionRef->intValue());
	}
	else if (c == brightness)
	{
		if (brightnessRef != nullptr) brightnessRef->setValue(brightness->floatValue());
	}
	else if (c == brightnessRef)
	{
		brightness->setValue(brightnessRef->floatValue());
	}
}

void BentoProp::serialDataReceived(SerialDevice* d, const var& data)
{
	//todo : parse to set sensors values
	if (logIncoming->boolValue())
	{
		NLOG(niceName + "(id " + globalID->stringValue() + ")", "Serial data received : " + data.toString());
	}
}

void BentoProp::portRemoved(SerialDevice* d)
{
	setSerialDevice(nullptr);
}


void BentoProp::sendColorsToPropInternal()
{
	int numChannels = colors.size() * 3;

	for (int i = 0; i < colors.size(); i++)
	{
		int index = i * 3;
		if (index >= DMX_NUM_CHANNELS - 2) break;
		Colour c = colors[i];
		float a = c.getFloatAlpha();
		data.set(index, jmin<int>(c.getRed() * a, 255));
		data.set(index + 1, jmin<int>(c.getGreen() * a, 255));
		data.set(index + 2, jmin<int>(c.getBlue() * a, 255));
	}
	universe.updateValues(data);
	if (universe.isDirty) artnet.sendDMXValues(&universe, numChannels);

}

void BentoProp::uploadPlaybackData(PlaybackData data)
{
	String target = "http://" + remoteHost->stringValue() + "/uploadFile";
	//String target = "http://benjamin.kuperberg.fr/chataigne/releases/uploadTest.php";

	NLOG(niceName, "Uploading " << target << " to " << data.name << " :\n > " << data.numFrames << " frames\n > " << (int)(data.data.getSize()) << " bytes");

	MemoryOutputStream mos;
	var mData = data.metaData;
	mData.getDynamicObject()->setProperty("fps", data.fps);
	mos.writeString(JSON::toString(data.metaData));
	MemoryBlock metaData = mos.getMemoryBlock();

	URL metaUrl = URL(target).withDataToUpload("uploadData", data.name + ".meta", metaData, "text/plain");

	std::unique_ptr<InputStream> mStream(metaUrl.createInputStream(URL::InputStreamOptions(URL::ParameterHandling::inPostData).withProgressCallback(std::bind(&BentoProp::uploadMetaDataProgressCallback, this, std::placeholders::_1, std::placeholders::_2)).withConnectionTimeoutMs(20000)));



	if (mStream != nullptr)
	{
		String response = mStream->readEntireStreamAsString();
		NLOG(niceName, "Metadata upload complete  (id " << String(globalID->intValue()) << ")");
	}
	else
	{
		NLOGERROR(niceName, "Error uploading meta data to prop (id " << String(globalID->intValue()) << "), stopping here.");
		return;
	}


	MemoryBlock dataToSend = data.data;

	MemoryOutputStream os;

	URL url;

	if (sendCompressedFile->boolValue())
	{
		MemoryInputStream* iis = new MemoryInputStream(data.data, true);
		ZipFile::Builder builder;
		builder.addEntry(iis, 9, "data", Time::getCurrentTime());

		builder.writeToStream(os, nullptr);
		dataToSend = os.getMemoryBlock();

	}

	sleep(500);

	url = URL(target).withDataToUpload("uploadData", data.name + ".colors", dataToSend, sendCompressedFile->boolValue() ? "application/zip" : "text/plain");

	std::unique_ptr<InputStream> stream(url.createInputStream(URL::InputStreamOptions(URL::ParameterHandling::inPostData).withProgressCallback(std::bind(&BentoProp::uploadProgressCallback, this, std::placeholders::_1, std::placeholders::_2)).withExtraHeaders("Content-Length:" + String(dataToSend.getSize())).withConnectionTimeoutMs(10000)));



	if (stream != nullptr)
	{
		String response = stream->readEntireStreamAsString();
		DBG("Got response : " << response);
		NLOG(niceName, "Upload complete");
	}
	else
	{
		NLOGERROR(niceName, "Error uploading color data to prop (id " << String(globalID->intValue()) << ")");
		return;
	}
}

void BentoProp::exportPlaybackData(PlaybackData data)
{
	MemoryInputStream is(data.data, true);
	if (exportFile.existsAsFile()) exportFile.deleteFile();
	FileOutputStream fs(exportFile);
	fs.writeFromInputStream(is, is.getDataSize());
	fs.flush();

	File mdFile = exportFile.getFileNameWithoutExtension() + ".meta";
	if (mdFile.existsAsFile()) mdFile.deleteFile();
	FileOutputStream mdfs(mdFile, true);
	mdfs.writeString(JSON::toString(data.metaData));
	mdfs.flush();

	ZipFile::Builder builder;
	builder.addFile(exportFile, 9, "data");
	File zf = exportFile.getSiblingFile(exportFile.getFileNameWithoutExtension() + "_compressed.zip");
	if (zf.existsAsFile()) zf.deleteFile();
	FileOutputStream fs2(zf);
	double progress = 0;
	builder.writeToStream(fs2, &progress);
}

void BentoProp::uploadFile(File f)
{
	String target = "http://" + remoteHost->stringValue() + "/uploadFile";
	FileInputStream fs(f);
	MemoryBlock b;
	fs.readIntoMemoryBlock(b);

	URL url = URL(target).withDataToUpload("uploadData", f.getFileName(), b, "text/plain");

	std::unique_ptr<InputStream> stream(url.createInputStream(URL::InputStreamOptions(URL::ParameterHandling::inPostData).withProgressCallback(std::bind(&BentoProp::uploadProgressCallback, this, std::placeholders::_1, std::placeholders::_2)).withExtraHeaders("Content-Length:" + String(b.getSize())).withConnectionTimeoutMs(10000)));



	if (stream != nullptr)
	{
		String response = stream->readEntireStreamAsString();
		DBG("Got response : " << response);
		NLOG(niceName, "Upload complete");
	}
	else
	{
		NLOGERROR(niceName, "Error uploading color data to prop (id " << String(globalID->intValue()) << ")");
		return;
	}
}

void BentoProp::loadPlayback(StringRef fileName, bool autoPlay)
{
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("player.load " + fileName + "\n");// +" " + (autoPlay ? "1" : "0") + " \n");
	}
	else
	{
		OSCMessage m(playbackAddress + "/load");
		m.addString(fileName);
		m.addInt32(autoPlay ? 1 : 0);
		sendMessageToProp(m);
	}
}

void BentoProp::playPlayback(float time, bool loop)
{
	if (serialDevice != nullptr)
	{
		String serialPlaybackAddress = playbackAddress.substring(1).replace("/", ".");
		serialDevice->writeString(serialPlaybackAddress + ".play\n");// +String(time == -1 ? -1 : time + .1f) + " \n");
	}
	else
	{

		OSCMessage m(playbackAddress + "/play");
		m.addFloat32(time);
		m.addInt32(loop ? 1 : 0);
		sendMessageToProp(m);
	}
}

void BentoProp::pausePlaybackPlaying()
{
	if (serialDevice != nullptr)
	{
		String serialPlaybackAddress = playbackAddress.substring(1).replace("/", ".");
		serialDevice->writeString(serialPlaybackAddress + ".pause\n");
	}
	else
	{
		OSCMessage m(playbackAddress + "/pause");
		sendMessageToProp(m);
	}
}

void BentoProp::seekPlaybackPlaying(float time)
{
	if (serialDevice != nullptr)
	{
		String serialPlaybackAddress = playbackAddress.substring(1).replace("/", ".");
		serialDevice->writeString(serialPlaybackAddress + "seek " + String(time) + "\n");
	}
	else
	{
		OSCMessage m(playbackAddress + "/seek");
		m.addFloat32(time);
		sendMessageToProp(m);
	}
}

void BentoProp::stopPlaybackPlaying()
{
	if (serialDevice != nullptr)
	{
		String serialPlaybackAddress = playbackAddress.substring(1).replace("/", ".");
		serialDevice->writeString(serialPlaybackAddress + ".stop\n");
	}
	else
	{
		OSCMessage m(playbackAddress + "/stop");
		sendMessageToProp(m);
	}
}

void BentoProp::sendShowPropID(bool value)
{
	if (serialDevice != nullptr)
	{
		String serialPlaybackAddress = playbackAddress.substring(1).replace("/", ".");
		serialDevice->writeString(serialPlaybackAddress + ".id " + String(value ? 1 : 0) + "\n");
	}
	else
	{
		OSCMessage m(playbackAddress + "/id");
		m.addInt32(value);
		sendMessageToProp(m);
	}
}

bool BentoProp::uploadProgressCallback(int bytesSent, int totalBytes)
{

	if (threadShouldExit()) return false;
	float p = bytesSent * 1.0f / totalBytes;
	uploadProgress->setValue(.1f + p * .9f);
	//NLOG(prop->niceName, "Uploading... " << (int)(prop->uploadProgress->floatValue() * 100) << "% (" << bytesSent << " / " << totalBytes << ")");

	return true;
}

bool BentoProp::uploadMetaDataProgressCallback(int bytesSent, int totalBytes)
{
	if (threadShouldExit()) return false;

	float p = bytesSent * 1.0f / totalBytes;
	uploadProgress->setValue(p * .1f);

	//NLOG(prop->niceName, "Uploading... " << (int)(prop->uploadProgress->floatValue() * 100) << "% (" << bytesSent << " / " << totalBytes << ")");

	return true;
}

void BentoProp::sendYo()
{
	OSCMessage m("/yo");
	m.addString(NetworkHelpers::getLocalIPForRemoteIP(remoteHost->stringValue()));
	sendMessageToProp(m);
}

void BentoProp::sendControlToProp(String control, var value)
{
	if (!enabled->boolValue()) return;
	if (logOutgoing->boolValue())
	{
		NLOG(niceName, "Sending " + control + " : " + value.toString());
	}

	OSCMessage m("/" + control.replaceCharacter('.', '/'));
	if (value.isArray())
	{
		for (int i = 0; i < value.size(); i++) m.addArgument(OSCHelpers::varToArgument(value[i], OSCHelpers::BoolMode::Int));
	}
	else if (!value.isVoid())
	{
		m.addArgument(OSCHelpers::varToArgument(value, OSCHelpers::BoolMode::Int));
	}

	sendMessageToProp(m);
}


void BentoProp::powerOffProp()
{
	NLOG(niceName, "Powering off");
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("root.sleep\n");
	}
	else
	{
		OSCMessage m("/root/sleep");
		sendMessageToProp(m);
	}
}

void BentoProp::restartProp()
{
	NLOG(niceName, "Restarting");
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("root.restart\n");
	}
	else
	{
		OSCMessage m("/root/restart");
		sendMessageToProp(m);
	}
}

void BentoProp::sendWiFiCredentials(String ssid, String pass)
{
	NLOG(niceName, "Set Wifi Credentials : " << ssid << ":" << pass);
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("wifi.setCredentials " + ssid + "," + pass + "\n");
	}
	else
	{
		OSCMessage m("/wifi/setCredentials");
		m.addString(ssid);
		m.addString(pass);
		sendMessageToProp(m);
	}
}

void BentoProp::sendMessageToProp(const OSCMessage& m)
{
	if (logOutgoing->boolValue())
	{
		String s = "Sending " + m.getAddressPattern().toString() + " : ";
		for (auto& a : m) s += "\n" + OSCHelpers::getStringArg(a);

		NLOG(niceName + " (id " + globalID->stringValue() + ")", s);
	}

	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
}

var BentoProp::sendMessageToPropFromScript(const var::NativeFunctionArgs& a)
{
	BentoProp* p = getObjectFromJS<BentoProp>(a);

	if (!checkNumArgs(p->niceName, a, 1)) return false;

	OSCMessage m(a.arguments[0].toString());
	for (int i = 1; i < a.numArguments; i++)
	{
		m.addArgument(OSCHelpers::varToArgument(a.arguments[i], OSCHelpers::BoolMode::Int));
	}

	p->sendMessageToProp(m);

	return true;
}