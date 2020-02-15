/*
  ==============================================================================

	BentoProp.cpp
	Created: 5 Mar 2019 11:01:56pm
	Author:  bkupe

  ==============================================================================
*/

#include "BentoProp.h"


BentoProp::BentoProp(StringRef name, StringRef family, var params) :
	Prop(name, family, params),
	serialDevice(nullptr)
{
	updateRate = 50;
	remoteHost = ioCC.addStringParameter("Remote Host", "IP of the prop on the network", "192.168.0.100");


	serialParam = new SerialDeviceParameter("Serial Device", "For connecting props trhough USB", true);
	ioCC.addParameter(serialParam);

	oscSender.connect("127.0.0.1", 1024);
}

BentoProp::~BentoProp()
{

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
		serialDevice->open(115200);
		if (!serialDevice->openedOk) serialDevice->addSerialDeviceListener(this);
		else NLOGERROR(niceName, "Error opening port " << serialDevice->info->description);
	}
}

void BentoProp::onContainerParameterChangedInternal(Parameter* p)
{
	Prop::onContainerParameterChangedInternal(p);

	if (p == globalID)
	{
		OSCMessage m("/settings/propID");
		m.addInt32(globalID->intValue());
		oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
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
			OSCMessage m("/strip/mode");
			m.addInt32(0);
			oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
		}

	}
	else if (c == serialParam)
	{
		setSerialDevice(serialParam->getDevice());
	}
}

void BentoProp::serialDataReceived(SerialDevice* d, const var&)
{
	LOG(niceName, "Serial Data : " << data.toString());
}


void BentoProp::sendColorsToPropInternal()
{
	const int numLeds = resolution->intValue();
	const int maxLedsPerPacket = 320;

	Array<uint8> data;

	int numPackets = 0;
	for (int i = 0; i < numLeds; i++)
	{
		float a = colors[i].getFloatAlpha();
		data.add(jmin<int>(colors[i].getRed() * a, 254));
		data.add(jmin<int>(colors[i].getGreen() * a, 254));
		data.add(jmin<int>(colors[i].getBlue() * a, 254));

		if (i % maxLedsPerPacket == 0) numPackets++;
	}

	data.add(255);

	int dataSize = numLeds * 3 + 1;

	for (int i = 0; i < numPackets; i++)
	{
		int offset = i * maxLedsPerPacket * 3;
		int length = i < numPackets - 1 ? maxLedsPerPacket * 3 : dataSize - offset;
		sender.write(remoteHost->stringValue(), remotePort, data.getRawDataPointer() + offset, length);
		sleep(1);
	}
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
		DBG("Got response : " << response);
		NLOG(niceName, "Metadata upload complete");
	}
	else
	{
		NLOGERROR(niceName, "Error uploading meta data to prop, stopping here.");
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
	std::unique_ptr<InputStream> stream(url.createInputStream(true, &BentoProp::uploadProgressCallback, this,"Content-Length:"+String(dataToSend.getSize()), 3000));// , "Content-Type: Text/plain");

	if (stream != nullptr)
	{
		String response = stream->readEntireStreamAsString();
		DBG("Got response : " << response);
		NLOG(niceName, "Upload complete");
	}
	else
	{
		NLOGERROR(niceName, "Error uploading color data to prop");
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
	OSCMessage m("/strip/mode");
	m.addInt32(2);
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);

	OSCMessage m2("/strip/player/load");
	m2.addString(fileName);
	m2.addInt32(autoPlay ? 1 : 0);
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m2);
}

void BentoProp::playBake(float time)
{
	OSCMessage m("/strip/player/play");
	m.addFloat32(time == -1 ? -1 : time + .1f);
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
}

void BentoProp::pauseBakePlaying()
{
	OSCMessage m("/strip/player/pause");
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
}

void BentoProp::seekBakePlaying(float time)
{
	OSCMessage m("/strip/player/seek");
	m.addFloat32(time);
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
}

void BentoProp::stopBakePlaying()
{
	OSCMessage m("/strip/player/stop");
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
}

void BentoProp::sendShowPropID(bool value)
{
	OSCMessage m("/strip/player/id");
	m.addInt32(value);
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
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

void BentoProp::sendPing()
{
	OSCMessage m("/ping");
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
}

void BentoProp::powerOffProp()
{
	NLOG(niceName, "Powering off");
	OSCMessage m("/sleep");
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
}

void BentoProp::resetProp()
{
	NLOG(niceName, "Resetting ");
	OSCMessage m("/reset");
	oscSender.sendToIPAddress(remoteHost->stringValue(), 9000, m);
}
