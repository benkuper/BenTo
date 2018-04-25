/*
  ==============================================================================

    VideoBlock.cpp
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

#include "VideoBlock.h"
#include "ui/VideoBlockUI.h"
#include "Video/Spatializer.h"

VideoBlock::VideoBlock(var params) :
	LightBlockModel(getTypeString(), params),
	receiver(nullptr)
{

	textureName = addStringParameter("Texture Name", "The Spout / Syphon name of the texture","");
	inputIsLive = addBoolParameter("Input is Live", "If there is something coming, you'll know it here", false);
	inputIsLive->setControllableFeedbackOnly(true);

	saveAndLoadName = false;
	saveType = false;
	
	setupReceiver();
}

VideoBlock::~VideoBlock()
{
	if(receiver != nullptr) receiver->removeListener(this);
}

void VideoBlock::setupReceiver()
{
	if (receiver != nullptr)
	{
		receiver->removeListener(this);
		SharedTextureManager::getInstance()->removeReceiver(receiver);
	}

	receiver = nullptr;

	receiver = SharedTextureManager::getInstance()->addReceiver(textureName->stringValue());

	if (receiver != nullptr)
	{
		receiver->addListener(this);
		receiver->createReceiver();
	}
	
}

Image VideoBlock::getImage()
{
	if (receiver == nullptr) return Image();
	else  return receiver->getImage();
}

Array<Colour> VideoBlock::getColors(int id, int resolution, float time, var params)
{
	if (Spatializer::getInstanceWithoutCreating() == nullptr || !inputIsLive->boolValue()) return LightBlockModel::getColors(id, resolution, time, params);
	SpatItem * spatItem = Spatializer::getInstance()->getItemWithPropID(id);

	if(spatItem == nullptr)  return LightBlockModel::getColors(id, resolution, time, params);

	int numSpatColors = spatItem->resolution->intValue();
	Array<Colour> result(spatItem->colors);
	result.resize(resolution);
	for (int i = resolution; i < numSpatColors; i++) result.set(i, Colours::black);

	return result;
}

void VideoBlock::textureUpdated(SharedTextureReceiver *)
{
	videoListeners.call(&VideoListener::textureUpdated, this);
}

void VideoBlock::connectionChanged(SharedTextureReceiver *)
{
	inputIsLive->setValue(receiver->isConnected);
}



void VideoBlock::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == textureName) setupReceiver();
}

LightBlockModelUI * VideoBlock::createUI()
{
	return new VideoBlockUI(this);
}
