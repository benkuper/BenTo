/*
  ==============================================================================

	BentoProp.cpp
	Created: 5 Mar 2019 11:01:56pm
	Author:  bkupe

  ==============================================================================
*/

#include "BentoProp.h"


BentoProp::BentoProp(var params) :
	Prop(params),
	serialDevice(nullptr)
{
	updateRate = params.getProperty("updateRate", 50);
	remoteHost = connectionCC.addStringParameter("Remote Host", "IP of the prop on the network", "192.168.0.100");

	serialParam = new SerialDeviceParameter("Serial Device", "For connecting props trhough USB", true);
	serialParam->openBaudRate = 115200;
	if (params.hasProperty("vid")) serialParam->vidFilter = (int)params.getProperty("vid", 0);
	if (params.hasProperty("pid")) serialParam->pidFilter = (int)params.getProperty("pid", 0);

	connectionCC.addParameter(serialParam);

	oscSender.connect("127.0.0.1", 1024);
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
		if(serialDevice->isOpen()) serialDevice->addSerialDeviceListener(this);
		else NLOGERROR(niceName, "Error opening port " << serialDevice->info->description);
	}
}

void BentoProp::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	Prop::onControllableFeedbackUpdateInternal(cc, c);
	if (c == bakeMode)
	{
		if (bakeMode->boolValue())
		{
			String filename = currentBlock != nullptr ? currentBlock->shortName : (bakeFileName->enabled ? bakeFileName->stringValue() : "");
			if (filename.isNotEmpty()) loadBake(filename);
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
		sendYo();
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
	const int numLeds = resolution->intValue();
	
	Array<uint8> data;

	for (int i = 0; i < numLeds; i++)
	{
		float a = colors[i].getFloatAlpha();
		data.add(jmin<int>(colors[i].getRed() * a, 254));
		data.add(jmin<int>(colors[i].getGreen() * a, 254));
		data.add(jmin<int>(colors[i].getBlue() * a, 254));
	}

	data.add(255);

	const int maxPacketSize = 1000; //1500 bytes on ESP32
	const int dataSize = data.size();
	int offset = 0;
	int numPacketSent = 0;

	while (offset < dataSize)
	{
		int length = jmin(maxPacketSize, dataSize - offset);

		int dataSent = sender.write(remoteHost->stringValue(), remotePort, data.getRawDataPointer() + offset, length);
		
		if (dataSent == -1)
		{
			LOGWARNING("Error sending data");
			break;
		}

		numPacketSent++;
		offset += dataSent;
		sleep(2);
	}

	if (numPacketSent > 1) sleep(10);
}

void BentoProp::uploadBakedData(BakeData data)
{
	String target = "http://" + remoteHost->stringValue() + "/upload";
	//String target = "http://benjamin.kuperberg.fr/chataigne/releases/uploadTest.php";

	NLOG(niceName, "Uploading " << target << " to " << data.name << " :\n > " << data.numFrames << " frames\n > " << (int)(data.data.getSize()) << " bytes");

	MemoryOutputStream mos;
	var mData = data.metaData;
	mData.getDynamicObject()->setProperty("fps", data.fps);
	mos.writeString(JSON::toString(data.metaData));
	MemoryBlock metaData = mos.getMemoryBlock();

	URL metaUrl = URL(target).withDataToUpload("uploadData", data.name + ".meta", metaData, "text/plain");

	std::unique_ptr<InputStream> mStream(metaUrl.createInputStream(true, &BentoProp::uploadMetaDataProgressCallback, this, "",20000));

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
	std::unique_ptr<InputStream> stream(url.createInputStream(true, &BentoProp::uploadProgressCallback, this,"Content-Length:"+String(dataToSend.getSize()), 10000));// , "Content-Type: Text/plain");

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

void BentoProp::exportBakedData(BakeData data)
{
	MemoryInputStream is(data.data, true);
	if (exportFile.existsAsFile()) exportFile.deleteFile();
	FileOutputStream fs(exportFile);
	fs.writeFromInputStream(is, is.getDataSize());
	fs.flush();

	ZipFile::Builder builder;
	builder.addFile(exportFile, 9, "data");
	File zf = exportFile.getSiblingFile(exportFile.getFileNameWithoutExtension() + "_compressed.zip");
	if (zf.existsAsFile()) zf.deleteFile();
	FileOutputStream fs2(zf);
	double progress = 0;
	builder.writeToStream(fs2, &progress);
}

void BentoProp::loadBake(StringRef fileName, bool autoPlay)
{
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("player.load " + fileName + "\n");// +" " + (autoPlay ? "1" : "0") + " \n");
	}
	else
	{
		OSCMessage m("/player/load");
		m.addString(fileName);
		m.addInt32(autoPlay ? 1 : 0);
		sendMessageToProp(m);
	}
}

void BentoProp::playBake(float time, bool loop)
{
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("player.play\n");// +String(time == -1 ? -1 : time + .1f) + " \n");
	}
	else
	{

		OSCMessage m("/player/play");
		m.addFloat32(time);
		m.addInt32(loop ? 1 : 0);
		sendMessageToProp(m);
	}
}

void BentoProp::pauseBakePlaying()
{
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("player.pause\n");
	}
	else
	{
		OSCMessage m("/player/pause");
		sendMessageToProp(m);
	}
}

void BentoProp::seekBakePlaying(float time)
{
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("player.seek "+String(time)+"\n");
	}
	else
	{
		OSCMessage m("/player/seek");
		m.addFloat32(time);
		sendMessageToProp(m);
	}
}

void BentoProp::stopBakePlaying()
{
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("player.stop\n");
	}
	else
	{
		OSCMessage m("/player/stop");
		sendMessageToProp(m);
	}
}

void BentoProp::sendShowPropID(bool value)
{
	if (serialDevice != nullptr)
	{
		serialDevice->writeString("player.id "+String(value?1:0)+"\n");
	}
	else
	{
		OSCMessage m("/player/id");
		m.addInt32(value);
		sendMessageToProp(m);
	}
}

bool BentoProp::uploadProgressCallback(void* context, int bytesSent, int totalBytes)
{
	BentoProp* prop = (BentoProp*)context;
	jassert(prop != nullptr);
	if (prop->threadShouldExit()) return false;
	float p = bytesSent * 1.0f / totalBytes;
	prop->uploadProgress->setValue(.1f + p * .9f);
	//NLOG(prop->niceName, "Uploading... " << (int)(prop->uploadProgress->floatValue() * 100) << "% (" << bytesSent << " / " << totalBytes << ")");

	return true;
}

bool BentoProp::uploadMetaDataProgressCallback(void* context, int bytesSent, int totalBytes)
{
	BentoProp* prop = dynamic_cast<BentoProp*>((BentoProp*)context);
	jassert(prop != nullptr);
	if (prop->threadShouldExit()) return false;

	float p = bytesSent * 1.0f / totalBytes;
	prop->uploadProgress->setValue(p * .1f);

	//NLOG(prop->niceName, "Uploading... " << (int)(prop->uploadProgress->floatValue() * 100) << "% (" << bytesSent << " / " << totalBytes << ")");

	return true;
}

void BentoProp::sendYo()
{
	OSCMessage m("/yo");
	m.addString(NetworkHelpers::getLocalIPForRemoteIP(remoteHost->stringValue()));
	sendMessageToProp(m);
}

void BentoProp::sendPingInternal()
{
	OSCMessage m("/ping");
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
		serialDevice->writeString("wifi.setCredentials "+ssid+","+pass+"\n");
	}
	else
	{
		OSCMessage m("/wifi/setCredentials");
		m.addString(ssid);
		m.addString(pass);
		sendMessageToProp(m);
	}
}

void BentoProp::sendControlToPropInternal(String control, var value)
{
	OSCMessage m("/"+control.replaceCharacter('.','/'));
	if (value.isArray())
	{
		for (int i = 0; i < value.size(); i++) m.addArgument(OSCHelpers::varToArgument(value[i]));
	}
	else if(!value.isVoid())
	{
		m.addArgument(OSCHelpers::varToArgument(value));
	}

	sendMessageToProp(m);
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
