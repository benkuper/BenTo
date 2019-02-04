/*
  ==============================================================================

    BentoWebServer.cpp
    Created: 4 Feb 2019 1:02:02pm
    Author:  jonglissimo

  ==============================================================================
*/

#include "BentoWebServer.h"

juce_ImplementSingleton(BentoWebServer)

BentoWebServer::BentoWebServer() :
	ControllableContainer("WebServer"),
	server(8080)
{
	setupRoutes();
	server.registerController(this);
	server.start();
}

BentoWebServer::~BentoWebServer()
{
}

void BentoWebServer::setupRoutes()
{
	addRoute("GET", "/getShow", BentoWebServer, getBakedShow);
}

void BentoWebServer::getBakedShow(Mongoose::Request & request, Mongoose::StreamResponse & response)
{
	String propID = htmlEntities(request.get("prop", ""));
	String showID = htmlEntities(request.get("show", ""));
	if (propID.isEmpty() || showID.isEmpty())
	{
		response << "You mush at least specity a prop and show parameters.";
		return;
	}

	response.setHeader("Content-Type", "application/octet-stream");
	
	MemoryOutputStream os;
	Random r;
	for (int i = 0; i < 55296000; i++) os.writeByte(r.nextInt()%255);
	/*MemoryInputStream is(os.getMemoryBlock(),true);
	DBG("Input stream " << is.getDataSize());
	ZipFile::Builder zipBuilder;
	zipBuilder.addEntry(&is, 9, "data", Time::getCurrentTime());
	MemoryOutputStream * zippedData = new MemoryOutputStream();
	double progress = 0;
	bool result = zipBuilder.writeToStream(*zippedData, nullptr);
	
	DBG("zip size " << (int)result << " : " << zippedData->getDataSize());
	*/
	String s = Base64::toBase64(os.getData(), os.getDataSize());
	response << s;
}
