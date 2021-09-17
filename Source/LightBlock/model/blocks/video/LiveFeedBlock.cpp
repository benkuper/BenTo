#include "LiveFeedBlock.h"
/*
  ==============================================================================

    LiveFeedBlock.cpp
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

LiveFeedBlock::LiveFeedBlock(var params) :
	TextureBlock(getTypeString(), params),
	receiver(nullptr)
{
	textureName = addStringParameter("Texture Name", "The Spout / Syphon name of the texture","");
	setupReceiver();

	SharedTextureManager::getInstance()->addListener(this);
}

LiveFeedBlock::~LiveFeedBlock()
{

	if (SharedTextureManager::getInstanceWithoutCreating() != nullptr)
	{
		if (receiver != nullptr)
		{
			receiver->removeListener(this);
			SharedTextureManager::getInstance()->removeReceiver(receiver);
			receiver = nullptr;
		}

		SharedTextureManager::getInstance()->removeListener(this);
	}

}

void LiveFeedBlock::setupReceiver()
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
		//receiver->setUseCPUImage(true);
		//receiver->createReceiver();
	}
	
}

Image LiveFeedBlock::getImage()
{
	if (receiver == nullptr || isClearing) return Image();
	else  return receiver->getImage();
}

void LiveFeedBlock::textureUpdated(SharedTextureReceiver*)
{
	Spatializer::getInstance()->computeSpat(getImage());
	//updateColorsFromImage();
}

void LiveFeedBlock::connectionChanged(SharedTextureReceiver*)
{
	inputIsLive->setValue(receiver->isConnected);
}

void LiveFeedBlock::receiverRemoved(SharedTextureReceiver* r)
{
	if(r == receiver) receiver = nullptr;
}

void LiveFeedBlock::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == textureName) setupReceiver();
}

LightBlockModelUI * LiveFeedBlock::createUI()
{
	return new LiveFeedBlockUI(this);
}
